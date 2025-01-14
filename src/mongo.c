#include <mongoc/mongoc.h>
#include <stdio.h>

void insert_file_metadata(const char *filename, const char *content) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_t *doc;

    mongoc_init();
    client = mongoc_client_new("mongodb://localhost:27017");
    collection = mongoc_client_get_collection(client, "sbd", "files");

    doc = bson_new();
    BSON_APPEND_UTF8(doc, "filename", filename);
    BSON_APPEND_UTF8(doc, "content", content);

    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, NULL)) {
        fprintf(stderr, "Failed to insert document\n");
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
