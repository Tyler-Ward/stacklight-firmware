
#include "stdint.h"

/**
 * @brief Configures the output pwm channels ready for use
 */
void SetupOutputs();

/**
 * @brief Set the outputs fom a DMX packet
 * 
 * @param offset DMX channel offset
 * @param data pointer to 512 byte DMX data packet
 */
void SetOutputsDMX(uint16_t offset, uint8_t* data);

/**
 * @brief Sets the outputs from a mode string
 * 
 * @param mode string pointer to desired mode
 */
void SetOutputsMode(char* mode);
