#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include "ElectricMeterSt.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET listenClient, listenWorker;
    struct sockaddr_in clientAddr, workerAddr;
    int cSize = sizeof(struct sockaddr_in);

    PriceSt prices = { 5, 10, 15 };
    WorkerInfo workers[MAX_WORKERS];
    TaskQueue queue;

    initQueue(&queue);
    for (int i = 0; i < MAX_WORKERS; i++) {
        workers[i].sock = INVALID_SOCKET;
        workers[i].state = FREE;
        workers[i].connected = 0;
    }

    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Socket za Client
    listenClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in clientListenAddr = { 0 };
    clientListenAddr.sin_family = AF_INET;
    clientListenAddr.sin_port = htons(5059);
    clientListenAddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenClient, (struct sockaddr*)&clientListenAddr, sizeof(clientListenAddr));
    listen(listenClient, 5);

    // Socket za Worker
    listenWorker = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in workerListenAddr = { 0 };
    workerListenAddr.sin_family = AF_INET;
    workerListenAddr.sin_port = htons(5060);
    workerListenAddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenWorker, (struct sockaddr*)&workerListenAddr, sizeof(workerListenAddr));
    listen(listenWorker, 5);

    printf("Server running...\n");

    while (1) {
        // Prihvatanje Workera
        SOCKET newWorkerSock = accept(listenWorker, (struct sockaddr*)&workerAddr, &cSize);
        if (newWorkerSock != INVALID_SOCKET) {
            int slot = -1;
            for (int i = 0; i < MAX_WORKERS; i++) if (!workers[i].connected) { slot = i; break; }
            if (slot != -1) {
                workers[slot].sock = newWorkerSock;
                workers[slot].state = FREE;
                workers[slot].connected = 1;

                printf("Worker #%d povezan, slanje cena...\n", slot);
                send(newWorkerSock, (char*)&prices, sizeof(prices), 0);

                char confirm[64] = { 0 };
                recv(newWorkerSock, confirm, sizeof(confirm), 0);
            }
            else {
                printf("Maksimalan broj Worker-a dostignut. Odbacujem.\n");
                closesocket(newWorkerSock);
            }
        }

        // Prihvatanje Client-a
        SOCKET clientSock = accept(listenClient, (struct sockaddr*)&clientAddr, &cSize);
        if (clientSock != INVALID_SOCKET) {
            ElectricMeterSt meter;
            int rec = recv(clientSock, (char*)&meter, sizeof(meter), 0);
            if (rec > 0) {
                TaskItem item;
                item.task = meter;
                item.clientSock = clientSock;
                if (!enqueue(&queue, &item)) {
                    printf("Queue pun, odbacujem zadatak.\n");
                    closesocket(clientSock);
                }
            }
            else {
                closesocket(clientSock);
            }
        }

        // Obrada queue-a i slanje zadataka Worker-ima
        for (int i = 0; i < MAX_WORKERS; i++) {
            if (workers[i].connected && workers[i].state == FREE && queue.count > 0) {
                TaskItem item;
                if (dequeue(&queue, &item)) {
                    workers[i].state = BUSY;
                    send(workers[i].sock, (char*)&item.task, sizeof(item.task), 0);

                    ResultSt result;
                    int got = recv(workers[i].sock, (char*)&result, sizeof(result), 0);
                    if (got > 0) {
                        printf("Rezultat od Worker #%d: %d\n", i, result.Result);
                        send(item.clientSock, (char*)&result, sizeof(result), 0);
                        closesocket(item.clientSock); // zatvori client konekciju
                    }

                    workers[i].state = FREE;
                }
            }
        }

        Sleep(50);
    }

    closesocket(listenClient);
    closesocket(listenWorker);
    WSACleanup();
    return 0;
}
