#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "mongo.h" // Include mongo.h for MongoDB-related functions

static int sbdfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void)fi;
    memset(stbuf, 0, sizeof(struct stat));
    // Implement MongoDB query here to fetch file/folder metadata and populate stbuf
    return 0;
}

static int sbdfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void)fi;
    int result = mongo_create_file(path, 0); // Initially, file size is set to 0
    return result == 0 ? 0 : -EEXIST;        // Return appropriate FUSE error codes
}

static int sbdfs_mkdir(const char *path, mode_t mode) {
    (void)mode; // Suppress unused parameter warning
    int result = mongo_create_directory(path);
    return result == 0 ? 0 : -EEXIST;
}

static int sbdfs_unlink(const char *path) {
    // Implement file deletion logic here using MongoDB
    return 0;
}

static int sbdfs_rmdir(const char *path) {
    // Implement folder deletion logic here using MongoDB
    return 0;
}

static int sbdfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                         struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void)offset;
    (void)fi;
    (void)flags;

    // Add logic to query MongoDB for the directory contents and fill them using filler
    filler(buf, ".", NULL, 0, 0); // Current directory
    filler(buf, "..", NULL, 0, 0); // Parent directory

    return 0;
}

static struct fuse_operations sbdfs_oper = {
    .getattr = sbdfs_getattr,
    .create = sbdfs_create,
    .mkdir = sbdfs_mkdir,
    .unlink = sbdfs_unlink,
    .rmdir = sbdfs_rmdir,
    .readdir = sbdfs_readdir,
};

int main(int argc, char *argv[]) {
    if (mongo_init("mongodb://localhost:27017", "sbdfs", "files") != 0) {
        fprintf(stderr, "Error initializing MongoDB.\n");
        return 1;
    }

    int ret = fuse_main(argc, argv, &sbdfs_oper, NULL);

    mongo_cleanup(); // Clean up MongoDB resources on exit
    return ret;
}
