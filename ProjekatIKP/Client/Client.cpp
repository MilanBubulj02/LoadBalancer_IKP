#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

void generateRandomData(ElectricMeterSt* meter) {
    srand((unsigned int)time(NULL) + rand());
    snprintf(meter->Id, ID_MAX_LEN, "A%d", rand() % 100 + 1);
    meter->BlueOld = rand() % 50;
    meter->GreenOld = rand() % 50;
    meter->RedOld = rand() % 50;
    meter->BlueNew = meter->BlueOld + (rand() % 50);
    meter->GreenNew = meter->GreenOld + (rand() % 50);
    meter->RedNew = meter->RedOld + (rand() % 50);
}

void printMeterData(ElectricMeterSt* meter) {
    printf("ID: %s\n", meter->Id);
    printf("BlueOld: %d, BlueNew: %d\n", meter->BlueOld, meter->BlueNew);
    printf("GreenOld: %d, GreenNew: %d\n", meter->GreenOld, meter->GreenNew);
    printf("RedOld: %d, RedNew: %d\n", meter->RedOld, meter->RedNew);
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    ElectricMeterSt meter;
    ResultSt result;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5059);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    while (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Cekam server...\n");
        Sleep(2000);
    }

    printf("Povezan sa serverom.\n");

    while (1) {
        printf("\nPritisnite Enter za novi zadatak...\n");
        getchar();

        generateRandomData(&meter);
        printMeterData(&meter);

        send(sock, (char*)&meter, sizeof(meter), 0);

        int bytes = 0;
        while ((bytes = recv(sock, (char*)&result, sizeof(result), 0)) <= 0) {
            Sleep(100);
        }

        printf("Rezultat od servera: ID=%s, Potrosnja=%d\n", result.Id, result.Result);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
