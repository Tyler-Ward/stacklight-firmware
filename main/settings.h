
#include "stdint.h"

typedef struct {
    uint16_t dmxAddr;
} settingsEEPROM_t;

uint16_t settingsGetDmxAddr();
void settingsSetDmxAddr(uint16_t dmxAddr);
