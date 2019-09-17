
#include "settings.h"

uint16_t dmxAddr = 1;

uint16_t settingsGetDmxAddr()
{
    return dmxAddr;
}

void settingsSetDmxAddr(uint16_t newdmxAddr)
{
    dmxAddr=newdmxAddr;
}
