#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    ElectricMeterSt meter;
    ResultSt result;
    PriceSt prices;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5060);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    while (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Cekam server...\n");
        Sleep(2000);
    }

    printf("Povezan sa serverom.\n");

    // Primanje cena
    recv(sock, (char*)&prices, sizeof(prices), 0);
    send(sock, "cene primljene", strlen("cene primljene") + 1, 0);

    while (1) {
        int bytes = recv(sock, (char*)&meter, sizeof(meter), 0);
        if (bytes <= 0) { Sleep(100); continue; }

        printf("Worker primio zadatak: ID=%s\n", meter.Id);

        // Obrada
        result.Result = ((meter.BlueNew - meter.BlueOld) * prices.BluePrice) +
            ((meter.GreenNew - meter.GreenOld) * prices.GreenPrice) +
            ((meter.RedNew - meter.RedOld) * prices.RedPrice);
        strcpy_s(result.Id, ID_MAX_LEN, meter.Id);

        send(sock, (char*)&result, sizeof(result), 0);
        printf("Rezultat poslat: %d\n", result.Result);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
