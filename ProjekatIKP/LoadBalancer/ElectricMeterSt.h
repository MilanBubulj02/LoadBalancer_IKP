#ifndef ELECTRIC_METER_ST_H
#define ELECTRIC_METER_ST_H

#define ID_SIZE 16

typedef struct {
    char Id[ID_SIZE];    // Može sadržati slova i brojeve
    int BlueOld;
    int GreenOld;
    int RedOld;
    int BlueNew;
    int GreenNew;
    int RedNew;
} ElectricMeterSt;

typedef struct {
    char Id[ID_SIZE];
    int BlueUsage;
    int GreenUsage;
    int RedUsage;
} ElectricMeterUsageSt;

#endif

