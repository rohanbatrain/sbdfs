{
  "path": "/path/to/file_or_directory",
  "name": "file_or_directory_name",
  "type": "file", // or "directory"
  "stat": {
    "st_mode": 33188,    // File mode
    "st_nlink": 1,       // Number of hard links
    "st_size": 123456,    // File size in bytes
    "st_uid": 1000,       // User ID of owner
    "st_gid": 1000,       // Group ID of owner
    "st_atime": 1674523012, // Last access time (in epoch)
    "st_mtime": 1674523012, // Last modification time (in epoch)
    "st_ctime": 1674523012  // Last change time (in epoch)
  }
}
