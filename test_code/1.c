#include <stdio.h>
#include <sys/stat.h>

int main() {
    // Combine file type and permissions using bitwise OR
    mode_t st_mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    // Print st_mode in octal format (file type and permissions)
    printf("st_mode: %o\n", st_mode);

    return 0;
}
