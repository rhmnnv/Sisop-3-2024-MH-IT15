#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define MAX_FILENAME_LENGTH 256

void authenticate_files() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir("new-data");
    if (!dir) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *filename = entry->d_name;
            if (strstr(filename, "trashcan") == NULL && strstr(filename, "parkinglot") == NULL) {
                // File name doesn't contain "trashcan" or "parkinglot", delete the file
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, MAX_FILENAME_LENGTH, "new-data/%s", filename);
                remove(filepath);
                printf("Deleted file: %s\n", filename);
            }
        }
    }
    closedir(dir);
}

void authenticate_and_move_to_shared_memory() {
    // Create or attach to shared memory segment
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

    DIR *dir;
    struct dirent *entry;
    dir = opendir("new-data");
    if (!dir) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }


    char *current_position = shared_memory;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *filename = entry->d_name;
            if (strstr(filename, ".csv") != NULL) { 
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, MAX_FILENAME_LENGTH, "new-data/%s", filename);

                FILE *file = fopen(filepath, "r");
                if (file == NULL) {
                    perror("Failed to open file");
                    continue; // Skip to the next file
                }

                strncpy(current_position, filename, MAX_FILENAME_LENGTH - (current_position - shared_memory));
                current_position += strlen(filename);
                *current_position++ = '\n'; // Add newline after filename

                char line[MAX_FILENAME_LENGTH];
                while (fgets(line, MAX_FILENAME_LENGTH, file) != NULL) {
                    strncpy(current_position, line, MAX_FILENAME_LENGTH - (current_position - shared_memory));
                    current_position += strlen(line);
                }

                *current_position++ = '\n';

                fclose(file);

                printf("Moved file content to shared memory: %s\n", filename);
            }
        }
    }
    closedir(dir);

    // Detach from shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    authenticate_files();
    authenticate_and_move_to_shared_memory();
    return 0;
}
