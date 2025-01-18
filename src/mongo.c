#include <mongoc/mongoc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

// Global MongoDB client and collection (to be initialized externally)
mongoc_client_t *client = NULL;
mongoc_collection_t *collection = NULL;

/**
 * Initializes the MongoDB connection and collection.
 * @param uri_string: MongoDB URI string (e.g., "mongodb://localhost:27017")
 * @param db_name: Name of the database to connect to
 * @param collection_name: Name of the collection to use
 * @return 0 on success, -1 on failure
 */
int mongo_init(const char *uri_string, const char *db_name, const char *collection_name) {
    mongoc_uri_t *uri;
    bson_error_t error;

    mongoc_init();
    uri = mongoc_uri_new_with_error(uri_string, &error);
    if (!uri) {
        fprintf(stderr, "Error: Failed to parse MongoDB URI: %s\n", error.message);
        return -1;
    }

    client = mongoc_client_new_from_uri(uri);
    if (!client) {
        fprintf(stderr, "Error: Failed to create MongoDB client.\n");
        mongoc_uri_destroy(uri);
        return -1;
    }

    collection = mongoc_client_get_collection(client, db_name, collection_name);
    if (!collection) {
        fprintf(stderr, "Error: Failed to get MongoDB collection.\n");
        mongoc_client_destroy(client);
        mongoc_uri_destroy(uri);
        return -1;
    }

    mongoc_uri_destroy(uri);
    return 0;
}

/**
 * Inserts file metadata into the MongoDB collection if it does not already exist.
 * 
 * @param path: Path of the file or directory to be inserted in the collection.
 * @param stbuf: A pointer to the stat structure containing file metadata (e.g., size, mode).
 * @return 0 on success, -EEXIST if the document already exists, -EIO on other errors.
 */
int mongo_insert_metadata(const char *path, const struct stat *stbuf) {
    bson_t stat;
    bson_t *query;
    bson_error_t error;

    // Extract file name from the path
    const char *name = strrchr(path, '/');
    name = (name) ? name + 1 : path; // Handle cases with or without '/'

    // Determine file type
    const char *type = S_ISDIR(stbuf->st_mode) ? "directory" : "file";

    BSON_INIT(&stat);

    BSON_APPEND_UTF8(&stat, "path", path);
    BSON_APPEND_UTF8(&stat, "name", name);
    BSON_APPEND_UTF8(&stat, "type", type);

    BSON_APPEND_DOCUMENT_BEGIN(&stat, "stat", &stat);
    BSON_APPEND_INT32(&stat, "st_mode", stbuf->st_mode);
    BSON_APPEND_INT32(&stat, "st_nlink", stbuf->st_nlink);
    BSON_APPEND_INT64(&stat, "st_size", stbuf->st_size);
    BSON_APPEND_INT32(&stat, "st_uid", stbuf->st_uid);
    BSON_APPEND_INT32(&stat, "st_gid", stbuf->st_gid);
    BSON_APPEND_TIME_T(&stat, "st_atime", stbuf->st_atime);
    BSON_APPEND_TIME_T(&stat, "st_mtime", stbuf->st_mtime);
    BSON_APPEND_TIME_T(&stat, "st_ctime", stbuf->st_ctime);
    BSON_APPEND_DOCUMENT_END(&stat);

    query = BCON_NEW("path", BCON_UTF8(path));

    if (mongoc_collection_find_one(collection, query, NULL, NULL, NULL, &error)) {
        fprintf(stderr, "Error: Document already exists for path %s.\n", path);
        bson_destroy(query);
        return -EEXIST;
    }

    if (!mongoc_collection_insert_one(collection, &stat, NULL, NULL, &error)) {
        fprintf(stderr, "Error: Failed to insert metadata into MongoDB: %s\n", error.message);
        bson_destroy(query);
        return -EIO;
    }

    bson_destroy(query);
    return 0;
}

/**
 * Updates file metadata in the MongoDB collection.
 * 
 * @param path: Path of the file or directory to be updated in the collection.
 * @param stbuf: A pointer to the stat structure containing file metadata (e.g., size, mode).
 * @return 0 on success, -EIO on failure (input/output error).
 */
int mongo_update_metadata(const char *path, const struct stat *stbuf) {
    bson_t stat;
    bson_t *query;
    bson_t *update;
    bson_error_t error;

    // Extract file name from the path
    const char *name = strrchr(path, '/');
    name = (name) ? name + 1 : path;

    // Determine file type
    const char *type = S_ISDIR(stbuf->st_mode) ? "directory" : "file";

    BSON_INIT(&stat);

    BSON_APPEND_DOCUMENT_BEGIN(&stat, "stat", &stat);
    BSON_APPEND_INT32(&stat, "st_mode", stbuf->st_mode);
    BSON_APPEND_INT32(&stat, "st_nlink", stbuf->st_nlink);
    BSON_APPEND_INT64(&stat, "st_size", stbuf->st_size);
    BSON_APPEND_INT32(&stat, "st_uid", stbuf->st_uid);
    BSON_APPEND_INT32(&stat, "st_gid", stbuf->st_gid);
    BSON_APPEND_TIME_T(&stat, "st_atime", stbuf->st_atime);
    BSON_APPEND_TIME_T(&stat, "st_mtime", stbuf->st_mtime);
    BSON_APPEND_TIME_T(&stat, "st_ctime", stbuf->st_ctime);
    BSON_APPEND_DOCUMENT_END(&stat);

    query = BCON_NEW("path", BCON_UTF8(path));
    update = BCON_NEW("$set", "{", 
        "path", BCON_UTF8(path), 
        "name", BCON_UTF8(name), 
        "type", BCON_UTF8(type), 
        "stat", BCON_DOCUMENT(&stat), 
    "}");

    if (!mongoc_collection_update_one(collection, query, update, NULL, NULL, &error)) {
        fprintf(stderr, "Error: Failed to update metadata in MongoDB: %s\n", error.message);
        bson_destroy(query);
        bson_destroy(update);
        return -EIO;
    }

    bson_destroy(query);
    bson_destroy(update);
    return 0;
}

/**
 * Queries MongoDB for all paths and extracts top-level directories.
 * 
 * @param root: The root directory where the structure starts ("/").
 * @return 0 on success, -1 on failure.
 */
int construct_directory_structure(const char *root) {
    bson_t query = BSON_INITIALIZER;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_iter_t iter;

    cursor = mongoc_collection_find_with_opts(collection, &query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &doc)) {
        if (bson_iter_init_find(&iter, doc, "path") && BSON_ITER_HOLDS_UTF8(&iter)) {
            const char *path = bson_iter_utf8(&iter, NULL);

            // Skip the root directory itself
            if (strcmp(path, "/") == 0) {
                continue;
            }

            // Extract top-level directory or file
            const char *sub_path = strchr(path + 1, '/');
            if (!sub_path) {
                sub_path = path + strlen(path); // No further slashes; use the full path
            }

            size_t length = sub_path - path;
            char top_level[length + 1];
            strncpy(top_level, path, length);
            top_level[length] = '\0';

            printf("Top-level directory: %s\n", top_level); // Debugging
            // You can now use `top_level` to create directories in FUSE
        }
    }

    if (mongoc_cursor_error(cursor, NULL)) {
        fprintf(stderr, "Error querying MongoDB for paths.\n");
        mongoc_cursor_destroy(cursor);
        return -1;
    }

    mongoc_cursor_destroy(cursor);
    return 0;
}

/**
 * Cleans up the MongoDB connection and collection resources.
 */
void mongo_cleanup() {
    if (collection) {
        mongoc_collection_destroy(collection);
    }
    if (client) {
        mongoc_client_destroy(client);
    }
    mongoc_cleanup();
}
