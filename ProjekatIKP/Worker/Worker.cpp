#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    ElectricMeterSt meter;
    ResultSt result;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_port = htons(5060);
        inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            printf("Waiting for server...\n");
            closesocket(sock);
            Sleep(2000);
            continue;
        }

        printf("Connected to server.\n");

        while (1) {
            int bytes = recv(sock, (char*)&meter, sizeof(meter), 0);
            if (bytes <= 0) break;

            result.Result = ((meter.BlueNew - meter.BlueOld) * 5) +
                ((meter.GreenNew - meter.GreenOld) * 10) +
                ((meter.RedNew - meter.RedOld) * 15);
            strcpy_s(result.Id, ID_MAX_LEN, meter.Id);

            send(sock, (char*)&result, sizeof(result), 0);
        }

        closesocket(sock);
        printf("Disconnected from server. Reconnecting...\n");
        Sleep(2000);
    }

    WSACleanup();
    return 0;
}
