
#include <stdint.h>

/**
 * @brief Process an RDM packet
 * 
 * @param buffer RDM packet without start code
 * @retval length of response packet 0 if no response packet
 */
int rdmProcessPacket(uint8_t* buffer);

/**
 * @brief Get RDM responce buffer
 * 
 * @retval RDM response packet without start code
 */
uint8_t* rdmGetBuffer();

/**
 * @brief Populates a buffer with the RDM UID
 * 
 * @param addr buffer to put RDM UID into
 */
void rdmGetRDMUID(uint8_t* addr);

