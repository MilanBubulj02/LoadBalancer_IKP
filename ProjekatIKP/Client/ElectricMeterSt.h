#ifndef ELECTRIC_METER_ST_H
#define ELECTRIC_METER_ST_H

#define ID_MAX_LEN 64

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

#endif



