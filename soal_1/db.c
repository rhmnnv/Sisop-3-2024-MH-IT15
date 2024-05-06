#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <time.h>

#define SHM_KEY 1234
#define MAX_FILENAME_LENGTH 256
#define MAX_LOG_LENGTH 512

void move_files_from_shared_memory() {
    int shmid = shmget(SHM_KEY, MAX_FILENAME_LENGTH * sizeof(char), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
  
    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    FILE *log_file = fopen("microservices/database/db.log", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    char timestamp[20];
    struct tm *tm_info;
    tm_info = localtime(&tv.tv_sec);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_info);

    char *current_position = shared_memory;
    while (*current_position != '\0') {
        // Find the end of the filename
        char *end_of_filename = strchr(current_position, '\n');
        if (end_of_filename == NULL) {
            fprintf(stderr, "Invalid format in shared memory: missing newline character\n");
            break;
        }

        *end_of_filename = '\0'; // Null-terminate the filename
        char *filename = current_position;

        // Move to the next filename (if any)
        current_position = end_of_filename + 1;

        char source_path[MAX_FILENAME_LENGTH];
        snprintf(source_path, sizeof(source_path), "new-data/%s", filename);
        char dest_path[MAX_FILENAME_LENGTH];
        snprintf(dest_path, sizeof(dest_path), "microservices/database/%s", filename);
      
        if (access(source_path, F_OK) != 0) {
            fprintf(stderr, "Source file '%s' does not exist\n", source_path);
            continue;
        }

        char* dest_directory = "microservices/database";
        if (access(dest_directory, F_OK) != 0) {
            fprintf(stderr, "Destination directory '%s' does not exist\n", dest_directory);
            continue;
        }

        if (rename(source_path, dest_path) == -1) {
            perror("Failed to move file");
            continue;
        }
        printf("Moved file from shared memory: %s\n", filename);

        char file_type[20];
        if (strstr(filename, "trashcan") != NULL) {
            strcpy(file_type, "[Trash Can]");
        } else if (strstr(filename, "parkinglot") != NULL) {
            strcpy(file_type, "[Parking Lot]");
        } else {
            strcpy(file_type, "[Unknown]");
        }

        char log_entry[MAX_LOG_LENGTH];
        snprintf(log_entry, sizeof(log_entry), "%s %s %s\n", timestamp, file_type, filename);
        fprintf(log_file, "%s", log_entry);
    }

    fclose(log_file);

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    move_files_from_shared_memory();
    return 0;
}
