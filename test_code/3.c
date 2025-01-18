#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

#define MAX_CONTENT_SIZE 1024  // Maximum buffer size for reading file content

// Structure to store file information
typedef struct {
    char name[256];              // File name without extension
    char extension[16];          // File extension (e.g., "txt", "md")
    char path[512];              // Absolute path of the file
    unsigned char content[MAX_CONTENT_SIZE];  // Buffer to store file content (text or binary)
    size_t content_size;         // Actual size of the content read from file
    struct stat stat;            // Structure holding file metadata (permissions, size, timestamps)
} FileDetails;

// Helper function to extract the file extension from the given filename
void get_extension(const char *filename, char *extension) {
    // Find the last dot (.) in the filename
    const char *dot = strrchr(filename, '.');
    if (dot && dot != filename) {
        strcpy(extension, dot + 1);  // Copy the extension part (after dot) into the extension variable
    } else {
        strcpy(extension, "none");  // No extension found
    }
}

// Helper function to remove the extension from the filename
void remove_extension(char *filename) {
    // Find the last dot (.) and replace it with a null terminator to remove the extension
    char *dot = strrchr(filename, '.');
    if (dot) {
        *dot = '\0';  // Null-terminate the string before the extension
    }
}

// Function to read file content and metadata into the FileDetails structure
int f(const char *filepath, FileDetails *file_details) {
    // Open the file in binary mode
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");  // Print error message if file can't be opened
        printf("File path: %s\n", filepath);  // Print the file path for debugging
        return -1;  // Return error code
    }

    // Extract the base name (last part of the path) of the file
    char *base_name = strrchr(filepath, '/');
    if (base_name == NULL) {
        base_name = (char *)filepath;  // If no '/', use the whole path as filename
    } else {
        base_name++;  // Skip the '/' character
    }

    // Copy the base file name and the absolute path into the structure
    strcpy(file_details->name, base_name);
    strcpy(file_details->path, filepath);

    // Remove the file extension from the file name
    remove_extension(file_details->name);

    // Get the file extension (e.g., txt, md)
    get_extension(file_details->name, file_details->extension);

    // Read file content based on the file extension
    size_t content_length = 0;
    if (strcmp(file_details->extension, "txt") == 0 || strcmp(file_details->extension, "md") == 0) {
        // For text files, read the content as text
        content_length = fread(file_details->content, 1, MAX_CONTENT_SIZE - 1, file);
        file_details->content[content_length] = '\0';  // Null-terminate the content for text files
    } else {
        // For binary files, read the raw bytes into the content buffer
        content_length = fread(file_details->content, 1, MAX_CONTENT_SIZE, file);
        file_details->content_size = content_length;  // Store actual content size for binary data
    }

    // Get file metadata using fstat
    int fd = fileno(file);  // Get the file descriptor associated with the file
    if (fstat(fd, &file_details->stat) == -1) {
        perror("Error getting file information");
        fclose(file);  // Close the file before returning
        return -1;  // Return error code
    }

    // Close the file after reading content and metadata
    fclose(file);
    return 0;  // Return success code
}

// Base64 encoding function for binary files (used for non-text files)
char* base64_encode(const unsigned char *data, size_t length) {
    const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t output_length = 4 * ((length + 2) / 3);  // Calculate the required size for the Base64-encoded string
    char *encoded_data = (char *)malloc(output_length + 1);  // Allocate memory for the encoded string
    if (encoded_data == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < length;) {
        unsigned int octet_a = i < length ? data[i++] : 0;
        unsigned int octet_b = i < length ? data[i++] : 0;
        unsigned int octet_c = i < length ? data[i++] : 0;

        unsigned int triple = (octet_a << 16) | (octet_b << 8) | octet_c;  // Combine 3 bytes into one 24-bit value

        encoded_data[j++] = base64_table[(triple >> 18) & 0x3F];  // First 6 bits
        encoded_data[j++] = base64_table[(triple >> 12) & 0x3F];  // Second 6 bits
        encoded_data[j++] = base64_table[(triple >> 6) & 0x3F];   // Third 6 bits
        encoded_data[j++] = base64_table[triple & 0x3F];          // Fourth 6 bits
    }

    // Add padding if necessary (for non-multiples of 3 bytes)
    for (size_t i = 0; i < (output_length - j); i++) {
        encoded_data[j + i] = '=';  // Add '=' padding characters
    }

    encoded_data[output_length] = '\0';  // Null-terminate the encoded string
    return encoded_data;  // Return the Base64-encoded string
}

// Helper function to print file details in JSON format
void print_file_details_json(const FileDetails *file_details) {
    // Output the file details as a JSON object
    printf("{\n");
    printf("  \"name\": \"%s\",\n", file_details->name);  // File name without extension
    printf("  \"extension\": \"%s\",\n", file_details->extension);  // File extension
    printf("  \"path\": \"/%s\",\n", file_details->path);  // Absolute file path

    if (file_details->content_size > 0) {
        // Print the content of the file if it exists
        if (strcmp(file_details->extension, "txt") != 0 && strcmp(file_details->extension, "md") != 0) {
            // For binary files, print the content as a Base64-encoded string
            printf("  \"content\": \"%s\",\n", base64_encode(file_details->content, file_details->content_size));
        } else {
            // For text files, print the content as is
            printf("  \"content\": \"%s\",\n", file_details->content);
        }
    }

    // Print file metadata as part of the JSON object
    printf("  \"stat\": {\n");
    printf("    \"st_mode\": %o,\n", file_details->stat.st_mode & 0777);  // Permissions in octal
    printf("    \"st_size\": %ld,\n", file_details->stat.st_size);  // File size
    printf("    \"st_uid\": %d,\n", file_details->stat.st_uid);  // User ID
    printf("    \"st_gid\": %d,\n", file_details->stat.st_gid);  // Group ID
    printf("    \"st_atime\": %ld,\n", file_details->stat.st_atime);  // Last access time
    printf("    \"st_mtime\": %ld,\n", file_details->stat.st_mtime);  // Last modification time
    printf("    \"st_ctime\": %ld\n", file_details->stat.st_ctime);  // Last change time
    printf("  }\n");
    printf("}\n");
}

// Main function to process the file and print details
int main(int argc, char *argv[]) {
    if (argc != 2) {
        // Check if the user provided a file path as a command-line argument
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;  // Exit with failure code if no file path is provided
    }

    const char *file_path = argv[1];  // Get the file path from the command-line argument
    FileDetails file_details;  // Declare a structure to hold the file details

    if (f(file_path, &file_details) == 0) {
        // If the file was successfully processed, print the details in JSON format
        print_file_details_json(&file_details);
    }

    return EXIT_SUCCESS;  // Exit with success code
}
