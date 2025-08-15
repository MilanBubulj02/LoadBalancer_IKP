#ifndef ELECTRIC_METER_ST_H
#define ELECTRIC_METER_ST_H

#include <winsock2.h>

#define ID_MAX_LEN 64
#define MAX_WORKERS 10

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

#endif
