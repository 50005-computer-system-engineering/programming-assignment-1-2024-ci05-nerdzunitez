#include "system_program.h"
#include <stdio.h>

int execute(char **args);
int move();

int main(){
    char *backup_dir_env = getenv("BACKUP_DIR");
    if (backup_dir_env == NULL) {
        fprintf(stderr, "Error: BACKUP_DIR environment variable is not set.\n");
        return 1;
    }

    char real_path[PATH_MAX];
    if (realpath(backup_dir_env, real_path) == NULL) {
        fprintf(stderr, "Error resolving absolute path of BACKUP_DIR.\n");
        return 1;
    }

    char *backup_dir = real_path;
    char *search_file = backup_dir;
    char *args[3];
    args[0] = "."; // Start searching from the current directory
    args[1] = search_file;
    args[2] = NULL; // Null-terminated array

    if (execute(args)) {
        char zip_command[512];
        // Assuming backup.zip is the desired output file name
        snprintf(zip_command, sizeof(zip_command), "zip -r backup.zip %s", backup_dir);
        // Execute the zip command
        int result = system(zip_command);
        if (result != 0) {
            fprintf(stderr, "Failed to zip the directory '%s'\n", backup_dir);
            return 1;
        } else {
            printf("Successfully zipped the directory '%s' into backup.zip\n", backup_dir);
            move();
        }
    } else {
        printf("No files found matching the keyword '%s'\n", search_file);
    }
    return 0;
}



int execute(char **args)
{
    if (args[1] == NULL)
    {
        printf("Usage: find [keyword], to find any matching filename in this directory or its children\n");
        return 1;
    }

    char *dir_name = args[0];
    char *to_match = args[1];
    int found = 0; // Track if the file is found

    DIR *d = opendir(dir_name ? dir_name : ".");
    if (!d)
    {
        fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name ? dir_name : ".", strerror(errno));
        return 1;
    }

    while (1)
    {
        struct dirent *entry = readdir(d);
        if (!entry)
        {
            break;
        }

        if (strstr(entry->d_name, to_match) != NULL)
        {
            printf("%s/%s\n", dir_name ? dir_name : ".", entry->d_name);
            found = 1; // Mark that a match was found
        }

        // Recursive search in subdirectories, excluding "." and ".."
        if (entry->d_type & DT_DIR && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0)
        {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dir_name ? dir_name : ".", entry->d_name);
            args[0] = path;
            if (execute(args) == 0)
            {
                found = 1; // Propagate the found status from the recursive call
            }
        }
    }

    closedir(d);
    return found ? 1 : 0; // Return 1 if found, otherwise 0
}


int move() {
    char *path_env = getenv("PATH");
    char *token;
    char project_path[1024] = {0};

    // Tokenize the PATH to find the project directory
    token = strtok(path_env, ":");
    while (token != NULL) {
        if (strstr(token, "programming-assignment-1-2024-ci05-nerdzunitez-main") != NULL) { 
            strcpy(project_path, token);
            break;
        }
        token = strtok(NULL, ":");
    }

    if (strlen(project_path) == 0) {
        fprintf(stderr, "Error: Project directory not found in PATH.\n");
        return 1;
    }

    // Create the "archive" directory within the project directory
    char archive_path[1024];
    snprintf(archive_path, sizeof(archive_path), "%s/archive", project_path);
    struct stat st = {0};
    if (stat(archive_path, &st) == -1) {
        if (mkdir(archive_path, 0700) == -1) {
            perror("mkdir() error");
            return 1;
        }
    }

    // Move the backup.zip file to the "archive" directory
    char new_zip_path[1024];
    snprintf(new_zip_path, sizeof(new_zip_path), "%s/archive/backup.zip", project_path);
    if (rename("backup.zip", new_zip_path) == -1) { // backup.zip is in the current working directory
        perror("rename() error");
        return 1;
    }

    printf("Successfully moved the zip file to '%s'\n", new_zip_path);
    return 0;
}