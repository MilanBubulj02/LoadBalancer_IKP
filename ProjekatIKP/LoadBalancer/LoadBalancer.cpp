#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET listenClient, listenWorker, clientSock, workerSock;
    struct sockaddr_in clientAddr, workerAddr;
    int cSize = sizeof(struct sockaddr_in);
    ElectricMeterSt meter;
    ResultSt result;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Listening for Client
    listenClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in clientListenAddr = { 0 };
    clientListenAddr.sin_family = AF_INET;
    clientListenAddr.sin_port = htons(5059);
    clientListenAddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenClient, (struct sockaddr*)&clientListenAddr, sizeof(clientListenAddr));
    listen(listenClient, 3);

    // Listening for Worker
    listenWorker = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in workerListenAddr = { 0 };
    workerListenAddr.sin_family = AF_INET;
    workerListenAddr.sin_port = htons(5060);
    workerListenAddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenWorker, (struct sockaddr*)&workerListenAddr, sizeof(workerListenAddr));
    listen(listenWorker, 3);

    printf("Server running...\n");

    workerSock = INVALID_SOCKET;

    while (1) {
        // Accept worker if not connected
        if (workerSock == INVALID_SOCKET) {
            workerSock = accept(listenWorker, (struct sockaddr*)&workerAddr, &cSize);
            if (workerSock != INVALID_SOCKET) {
                printf("Worker connected.\n");
            }
        }

        clientSock = accept(listenClient, (struct sockaddr*)&clientAddr, &cSize);
        if (clientSock == INVALID_SOCKET) continue;

        int rec = recv(clientSock, (char*)&meter, sizeof(meter), 0);
        if (rec > 0 && workerSock != INVALID_SOCKET) {
            send(workerSock, (char*)&meter, sizeof(meter), 0);
            int got = recv(workerSock, (char*)&result, sizeof(result), 0);
            if (got > 0) {
                send(clientSock, (char*)&result, sizeof(result), 0);
            }
        }
        closesocket(clientSock);
    }

    closesocket(listenClient);
    closesocket(listenWorker);
    WSACleanup();
    return 0;
}
