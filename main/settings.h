
#include "stdint.h"

/**
 * @brief Setup the settings manager
 */
void settingsSetup();

/**
 * @brief Get the DMX channel offset
 * 
 * @retval DMX channel offset
 */
uint16_t settingsGetDmxAddr();
/**
 * @brief Set the DMX channel offset
 * 
 * @param dmxAddr DMX offset channel offset
 */
void settingsSetDmxAddr(uint16_t dmxAddr);

/**
 * @brief Get the Artnet Net address
 * 
 * @retval Artnet Net address
 */
uint8_t settingsGetArtnetNet();
/**
 * @brief Set the Artnet Net address
 * 
 * @param newArtnetNet Artnet Net address
 */
void settingsSetArtnetNet(uint8_t newArtnetNet);

/**
 * @brief Get the Artnet Subnet address
 * 
 * @retval Artnet Subnet address
 */
uint8_t settingsGetArtnetSubNet();
/**
 * @brief Set the Artnet Net address
 * 
 * @param newArtnetSubNet Artnet Subnet address
 */
void settingsSetArtnetSubNet(uint8_t newArtnetSubNet);

/**
 * @brief Get the Artnet Universe
 * 
 * @retval Artnet universe
 */
uint8_t settingsGetArtnetUniverse();
/**
 * @brief Set the Artnet Net address
 * 
 * @param newArtnetUniverse Artnet universe
 */
void settingsSetArtnetUniverse(uint8_t newArtnetUniverse);

/**
 * @brief Get the Brightness
 * 
 * @retval brightness value 0-255
 */
uint8_t settingsGetBrightness();
/**
 * @brief Set the Brightness
 * 
 * @param newBrightness brightness value 0-255
 */
void settingsSetBrightness(uint8_t newBrightness);

/**
 * @brief Get the idle Timout
 * 
 * @retval timeout in seconds (0 is disabled)
 */
uint16_t settingsGetIdleModeTimeout();
/**
 * @brief Set the idle Timout
 * 
 * @param newIdleModeTimeout timeout in seconds (0 is disabled)
 */
void settingsSetIdleModeTimeout(uint16_t newIdleModeTimeout);

/**
 * @brief Get the idle mode
 * 
 * @retval string containing the idle Mode
 */
char* settingsGetidleMode();
/**
 * @brief Set the idle mode
 * 
 * @param newIdleMode string containing the idle Mode
 */
void settingsSetIdleMode(char* newIdleMode);