
#include "stdint.h"

void settingsSetup();

uint16_t settingsGetDmxAddr();
void settingsSetDmxAddr(uint16_t dmxAddr);

uint8_t settingsGetArtnetNet();
void settingsSetArtnetNet(uint8_t newArtnetNet);

uint8_t settingsGetArtnetSubNet();
void settingsSetArtnetSubNet(uint8_t newArtnetSubNet);

uint8_t settingsGetArtnetUniverse();
void settingsSetArtnetUniverse(uint8_t newArtnetUniverse);