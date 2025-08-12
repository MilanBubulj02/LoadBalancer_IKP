#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 5159

int main() {
    WSADATA wsaData;
    SOCKET listenSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listenSocket, 1);

    printf("Server ceka konekciju...\n");
    clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    printf("Klijent povezan!\n");

    ElectricMeterSt meter;
    recv(clientSocket, (char*)&meter, sizeof(meter), 0);

    printf("Primljeni podaci:\n");
    printf("ID: %s\n", meter.Id);
    printf("BlueOld: %d, BlueNew: %d\n", meter.BlueOld, meter.BlueNew);
    printf("GreenOld: %d, GreenNew: %d\n", meter.GreenOld, meter.GreenNew);
    printf("RedOld: %d, RedNew: %d\n", meter.RedOld, meter.RedNew);

    ElectricMeterUsageSt usage;
    snprintf(usage.Id, ID_SIZE, "%s", meter.Id);
    usage.BlueUsage = meter.BlueNew - meter.BlueOld;
    usage.GreenUsage = meter.GreenNew - meter.GreenOld;
    usage.RedUsage = meter.RedNew - meter.RedOld;

    send(clientSocket, (char*)&usage, sizeof(usage), 0);

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
