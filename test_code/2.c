#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    int st_mode;
    int st_nlink;
    long st_size;
    int st_uid;
    int st_gid;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
} Stat;

// Function to populate the Stat structure
void populate_stat(Stat *stat, int mode, int nlink, long size, int uid, int gid, time_t atime, time_t mtime, time_t ctime) {
    if (stat == NULL) return;

    stat->st_mode = mode;
    stat->st_nlink = nlink;
    stat->st_size = size;
    stat->st_uid = uid;
    stat->st_gid = gid;
    stat->st_atime = atime;
    stat->st_mtime = mtime;
    stat->st_ctime = ctime;
}

// Function to print the Stat structure for debugging
void print_stat(const Stat *stat) {
    if (stat == NULL) return;

    printf("st_mode: %d\n", stat->st_mode);
    printf("st_nlink: %d\n", stat->st_nlink);
    printf("st_size: %ld\n", stat->st_size);
    printf("st_uid: %d\n", stat->st_uid);
    printf("st_gid: %d\n", stat->st_gid);
    printf("st_atime: %ld\n", stat->st_atime);
    printf("st_mtime: %ld\n", stat->st_mtime);
    printf("st_ctime: %ld\n", stat->st_ctime);
}

int main() {
    Stat stat;
    time_t current_time = time(NULL);

    // Populate the structure
    populate_stat(&stat, 0755, 2, 1024, 1000, 1000, current_time, current_time, current_time);

    // Print the structure
    print_stat(&stat);

    return 0;
}
