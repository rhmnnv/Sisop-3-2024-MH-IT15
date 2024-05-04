#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include "actions.c"

#define PORT 8080
#define LOG_FILE "race.log"

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    //waktu
    time_t current_time;
    struct tm* time_info;

    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setting up the address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }


    while(1) {

        time(&current_time);
        time_info = localtime(&current_time);

        FILE* file_ptr = fopen(LOG_FILE, "a");
        if (file_ptr == NULL) {
            perror("Failed to open log file");
            close(new_socket);
            continue;
        }


        // Accepting incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        bzero(buffer, 1024);
        recv(new_socket, buffer, sizeof(buffer), 0);

        printf("Connection accepted\n");
        const char *result;

         if (strcmp("Gap", buffer) == 0)
        {
            float terima;
            recv(new_socket, &terima, sizeof(terima), 0);
            result=gap(terima);
        }
        else if (strcmp("Fuel", buffer) == 0)
        {
            int terima;
            recv(new_socket, &terima, sizeof(terima), 0);
            result=fuel(terima);
        }
        else if (strcmp("Sisa", buffer) == 0)
        {
            int terima;
            recv(new_socket, &terima, sizeof(terima), 0);
            result=ban(terima);
        }
        else if (strcmp("Tire", buffer) == 0)
        {  
            
            result=tire(buffer);
        }

        fprintf(file_ptr, "[Paddock] [%02d/%02d/%02d %02d:%02d:%02d] [%s]: %s\n",
            time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           buffer,
           result);

        fclose(file_ptr);

        printf("%s\n", result);
        close(new_socket); 
    }

    return 0;
}
