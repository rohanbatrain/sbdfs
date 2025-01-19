#ifndef SBDFS_H
#define SBDFS_H

#include <fuse.h>

// FUSE operation functions
int sbdfs_getattr(const char *path, struct stat *stbuf);
int sbdfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

#endif
