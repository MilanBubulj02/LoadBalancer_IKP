#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <time.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 5159
#define SERVER_IP "127.0.0.1"

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    InetPtonA(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Ne mogu da se povezem na server.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    srand((unsigned int)time(NULL));

    ElectricMeterSt meter;
    snprintf(meter.Id, ID_SIZE, "A%d", rand() % 10 + 1);

    meter.BlueOld = rand() % 50;
    meter.GreenOld = rand() % 50;
    meter.RedOld = rand() % 50;

    meter.BlueNew = meter.BlueOld + (rand() % 50);
    meter.GreenNew = meter.GreenOld + (rand() % 50);
    meter.RedNew = meter.RedOld + (rand() % 50);

    printf("Slanje podataka serveru:\n");
    printf("ID: %s\n", meter.Id);
    printf("BlueOld: %d, BlueNew: %d\n", meter.BlueOld, meter.BlueNew);
    printf("GreenOld: %d, GreenNew: %d\n", meter.GreenOld, meter.GreenNew);
    printf("RedOld: %d, RedNew: %d\n", meter.RedOld, meter.RedNew);

    send(clientSocket, (char*)&meter, sizeof(meter), 0);

    ElectricMeterUsageSt usage;
    recv(clientSocket, (char*)&usage, sizeof(usage), 0);

    printf("\nRezultat sa servera:\n");
    printf("ID: %s\n", usage.Id);
    printf("Blue Usage: %d\n", usage.BlueUsage);
    printf("Green Usage: %d\n", usage.GreenUsage);
    printf("Red Usage: %d\n", usage.RedUsage);

    getchar();
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}


