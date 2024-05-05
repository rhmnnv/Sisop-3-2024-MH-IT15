#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include "actions.c"

#define PORT 8080
#define LOG_FILE "/soal_3/race.log"

void penulis(const char *orang, const char *jenis, const char *jumlah) {

    FILE* file_ptr = fopen(LOG_FILE, "a");
    if (file_ptr == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    fprintf(file_ptr, "[%s] [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n",
            orang,
            local_time->tm_mday, 
            local_time->tm_mon + 1, 
            local_time->tm_year + 1900,
            local_time->tm_hour, 
            local_time->tm_min, 
            local_time->tm_sec,
            jenis, 
            jumlah);
    printf("it works\n");
    fclose(file_ptr);
}

void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


int main() {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    daemonize();
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Clear buffer
        bzero(buffer, sizeof(buffer));

        // Receive data from client
        if (recv(new_socket, buffer, sizeof(buffer), 0) < 0) {
            perror("Receive failed");
            close(new_socket);
            continue;
        }

        const char *result;

        if (strcmp("Gap", buffer) == 0) {
            float distance;
            if (recv(new_socket, &distance, sizeof(distance), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_distance[100];
            sprintf(str_distance, "%.3f", distance);
            result = gap(distance);
            penulis("[Driver]", buffer, str_distance);

        } else if (strcmp("Fuel", buffer) == 0) {
            int bensin;
            if (recv(new_socket, &bensin, sizeof(bensin), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_fuel[100];
            sprintf(str_fuel, "%d", bensin);
            result = fuel(bensin);
            penulis("[Driver]", buffer, str_fuel);

        } else if (strcmp("Sisa", buffer) == 0) {
            int sisa;
            if (recv(new_socket, &sisa, sizeof(sisa), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_sisa[100];
            sprintf(str_sisa, "%d", sisa);
            result = ban(sisa);
            penulis("[Driver]", buffer, str_sisa);

        } else if (strcmp("Tire", buffer) == 0) {
            char tire_type[256];
            if (recv(new_socket, tire_type, sizeof(tire_type) - 1, 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char jenis_ban[100];
            strncpy(jenis_ban, tire_type, 6);
            jenis_ban[4] = '\0';
            if (strcmp(jenis_ban, "Medi") == 0) {
                strcpy(jenis_ban, "Medium");
            }
            result = tire(jenis_ban);
            penulis("[Driver]", buffer, jenis_ban);

        } else {
            result = "Wrongggg Command";
            penulis("[Driver]", buffer, "ERROR");
            
        }

        penulis("[Paddock]", buffer, result);

        // Send response to client
        if (send(new_socket, result, strlen(result), 0) < 0) {
            perror("Send failed");
        }

        // Close socket
        close(new_socket);
    }

    return 0;
}
