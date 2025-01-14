#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h> // Include this header for memset and strcmp

static int sbdfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi; // Suppress unused parameter warning
    memset(stbuf, 0, sizeof(struct stat)); // Clear the stat structure

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755; // Directory with permissions
        stbuf->st_nlink = 2;            // Two links (current and parent directory)
    } else {
        stbuf->st_mode = S_IFREG | 0444; // Regular file with read-only permissions
        stbuf->st_nlink = 1;            // One link (the file itself)
        stbuf->st_size = 0;             // Zero size for now
    }
    return 0;
}

static struct fuse_operations sbdfs_oper = {
    .getattr = sbdfs_getattr,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &sbdfs_oper, NULL);
}
