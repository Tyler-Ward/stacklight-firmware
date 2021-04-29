// functions to control status indicators

#include <stdbool.h>

/**
 * @brief Initialize the indicators.
 */
void indicatorsSetup();

/**
 * @brief Set the state of the Artnet Indicator.
 * 
 * @param iluminated 1 if the led should be lot 0 if not
 */
void indicatorsSetArtnet(bool iluminated);

/**
 * @brief Set the state of the locate Indicator.
 * 
 * @param iluminated 1 if the led should be lot 0 if not
 */
void indicatorsSetLocate(bool iluminated);

/**
 * @brief Gets the state of the locate Indicator.
 * 
 * @retval 1 if the led should be lot 0 if not
 */
bool indicatorsGetLocate();

/**
 * @brief Set the state of the network Indicator.
 * 
 * @param iluminated 1 if the led should be lot 0 if not
 */
void indicatorsSetNetwork(bool iluminated);

/**
 * @brief Set the state of the status Indicator.
 * 
 * @param iluminated 1 if the led should be lot 0 if not
 */
void indicatorsSetStatus(bool iluminated);