#ifndef MONGO_H
#define MONGO_H

#include <mongoc/mongoc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

// Function prototypes
int mongo_init(const char *uri_string, const char *db_name, const char *collection_name);
int mongo_insert_metadata(const char *path, const struct stat *stbuf);
int mongo_update_metadata(const char *path, const struct stat *stbuf);
int mongo_create_directory(const char *path);
int mongo_create_file(const char *path, size_t size);
void mongo_cleanup();

#endif // MONGO_H
