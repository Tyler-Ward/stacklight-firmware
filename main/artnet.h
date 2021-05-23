#include <stdint.h>

#define ARTNET_ACTION_NONE 0x00 //no reply required
#define ARTNET_ACTION_SEND_REPLY 0x01 //send reply

/**
 * @brief processes a recieved artnet packet
 * 
 * @param packet pointer to recieved UDP payload
 * @param length UDP layload length
 * @retval action code to be carried out after packet processing
 */
int artnetProcessPacket(uint8_t* packet, unsigned int length);

/**
 * @brief Get length of artnet response
 * 
 * @retval length of artnet response loaded into packet buffer
 */
int artnetReplyLen();

/**
 * @brief Set Artnet IP Address
 * 
 * Sets the IPv4 address used in artpollreply messages
 * 
 * @param address pointer to 4 byte IPv4 address
 */
void artnetSetIpAddress(uint8_t* address);

/**
 * @brief Set Artnet Mac Address
 * 
 * Sets the MAC address used in artpollreply messages
 * 
 * @param address pointer to 6 byte MAC address
 */
void artnetSetMacAddress(uint8_t* address);

