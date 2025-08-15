#ifndef ELECTRIC_METER_ST_H
#define ELECTRIC_METER_ST_H

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define ID_MAX_LEN 64
#define MAX_WORKERS 10
#define MAX_QUEUE 100

typedef struct ElectricMeterSt {
    char Id[ID_MAX_LEN];
    int BlueOld;
    int GreenOld;
    int RedOld;
    int BlueNew;
    int GreenNew;
    int RedNew;
} ElectricMeterSt;

typedef struct ResultSt {
    char Id[ID_MAX_LEN];
    int Result;
} ResultSt;

typedef struct PriceSt {
    int BluePrice;
    int GreenPrice;
    int RedPrice;
} PriceSt;

typedef enum {
    FREE = 0,
    BUSY = 1
} WorkerState;

typedef struct WorkerInfo {
    SOCKET sock;
    WorkerState state;
    int connected;
} WorkerInfo;

// Queue item: zadatak + client socket
typedef struct TaskItem {
    ElectricMeterSt task;
    SOCKET clientSock;
} TaskItem;

typedef struct TaskQueue {
    TaskItem tasks[MAX_QUEUE];
    int front;
    int rear;
    int count;
} TaskQueue;

static void initQueue(TaskQueue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

static int enqueue(TaskQueue* q, TaskItem* item) {
    if (q->count >= MAX_QUEUE) return 0;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->tasks[q->rear] = *item;
    q->count++;
    return 1;
}

static int dequeue(TaskQueue* q, TaskItem* item) {
    if (q->count <= 0) return 0;
    *item = q->tasks[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    q->count--;
    return 1;
}

#endif
