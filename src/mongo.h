#ifndef MONGO_H
#define MONGO_H

#include <mongoc/mongoc.h>
#include <sys/stat.h>

// Function declarations
int mongo_init(const char *uri_string, const char *db_name, const char *collection_name);
int mongo_insert_metadata(const char *path, const struct stat *stbuf);
int mongo_update_metadata(const char *path, const struct stat *stbuf);
int construct_directory_structure(const char *root);
void mongo_cleanup();

#endif // MONGO_H
