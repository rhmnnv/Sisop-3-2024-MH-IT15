#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define MAX_FILENAME_LENGTH 256

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    float rating;
} Place;

void get_highest_rating(const char *type) {
    // Variables to store the highest rated place for each type
    float highest_rating = 0;
    char highest_name[MAX_FILENAME_LENGTH];
    char highest_filename[MAX_FILENAME_LENGTH];
  
    int shmid = shmget(SHM_KEY, MAX_FILENAME_LENGTH * sizeof(char), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    char *current_position = shared_memory;
    while (*current_position != '\0') {
        char filename[MAX_FILENAME_LENGTH];
        strcpy(filename, current_position);
        current_position += strlen(filename) + 1; // Move to the next filename

        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            perror("Failed to open file");
            continue; // Skip to the next file
        }

        Place place;
        while (fscanf(file, "%[^,], %f\n", place.name, &place.rating) != EOF) {
            if (place.rating > highest_rating) {
                highest_rating = place.rating;
                strcpy(highest_name, place.name);
                strcpy(highest_filename, filename);
            }
        }
        fclose(file);
    }

    printf("Type: %s\n", type);
    printf("Filename: %s\n", highest_filename);
    printf("--------------------\n");
    printf("Name: %s\n", highest_name);
    printf("Rating: %.1f\n", highest_rating);

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    get_highest_rating("Trash Can");
    get_highest_rating("Parking Lot");
    return 0;
}
