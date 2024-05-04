#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define IP "127.0.0.1"

int main(int argc, char *argv[]) {
    char *jenis = argv[1];
    char *jumlah = argv[2];

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, IP, &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Koneksi Berhasil\n");


    bzero(buffer, 1024);
    strcpy(buffer, jenis);
    send(sock, buffer, strlen(buffer), 0);

    if (strcmp("Gap", jenis)== 0)
    {
        float angka = atof(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Fuel", jenis)== 0)
    {
        int angka = atoi(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Sisa", jenis)== 0)
    {
        int angka = atoi(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Tire", jenis)== 0)
    {
        if(strcmp("Medium", jumlah)== 0){
            bzero(buffer, 1024);
            strcpy(buffer, jumlah);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (strcmp("Soft", jumlah)== 0){
            bzero(buffer, 1024);
            strcpy(buffer, jumlah);
            send(sock, buffer, strlen(buffer), 0);
        }
    }
    
}
