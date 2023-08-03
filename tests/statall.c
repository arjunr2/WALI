#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void printFilesRecursively(const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(dirname)) == NULL) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Recursive call for directories
            printFilesRecursively(path);
        } else {
            volatile int i = 0;
            // Use stat syscall to get file information
            if (stat(path, &file_stat) == 0) {
                i++;
                //printf("File: %s, Size: %ld bytes\n", path, file_stat.st_size);
                // You can access other file information from the 'file_stat' structure
            } else {
                i++;
                //perror("Error getting file information");
            }
        }
    }

    closedir(dir);
}

int main() {
    char currentDir[1024];

    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        perror("Error getting current directory");
        return EXIT_FAILURE;
    }

    printf("Files in the current directory:\n");
    printFilesRecursively(currentDir);

    return EXIT_SUCCESS;
}

