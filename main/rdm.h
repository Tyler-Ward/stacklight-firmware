
#include <stdint.h>

#pragma pack(push, 1)

typedef struct
{
    uint8_t subStartCode;
    uint8_t messageLength;
    uint8_t destination[6];
    uint8_t source[6];
    uint8_t transactionNumber;
    uint8_t portID; //also used for response type
    uint8_t messageCount;
    uint16_t subDevice;
    uint8_t data[];
} rdm_t;

typedef struct
{
    uint8_t subStartCode;
    uint8_t messageLength;
    uint8_t destination[6];
    uint8_t source[6];
    uint8_t transactionNumber;
    uint8_t portID; //also used for response type
    uint8_t messageCount;
    uint16_t subDevice;
    uint8_t commandClass;
    uint16_t parameterID;
    uint8_t parameterDataLength;
    uint8_t parameterData[];
} rdm_sub_message_t;

#pragma pack(pop)

int processRdm(rdm_t* rdm);
int rdm_generate_nack_reason(rdm_sub_message_t* recieved, uint16_t reason);
uint8_t finalisePacket(rdm_sub_message_t* packet);
uint8_t* rdmgetBuffer();
void getRDMUID(uint8_t* addr);

#define RDM_SC_RDM 0xCC
#define RDM_SC_SUB_MESSAGE 0x01

//A1 - command class DEFINES

#define RDM_DISCOVERY_COMMAND 0x10
#define RDM_DISCOVERY_COMMAND_RESPONSE 0x11
#define RDM_GET_COMMAND 0x20
#define RDM_GET_COMMAND_RESPONSE 0x21
#define RDM_SET_COMMAND 0x30
#define RDM_SET_COMMAND_RESPONSE 0x31

//A2 - response type defines

#define RDM_RESPONSE_TYPE_ACK 0x00
#define RDM_RESPONSE_TYPE_ACK_TIMER 0x01
#define RDM_RESPONSE_TYPE_NACK_REASON 0x02
#define RDM_RESPONSE_TYPE_ACK_OVERFLOW 0x03

//A3 - RDM parameter IDs (incomplete)

#define RDM_SUPPORTED_PARAMETERS 0x0050
#define RDM_PARAMETER_DESCRIPTION 0x0051

#define RDM_DEVICE_INFO 0x0060
#define RDM_DEVICE_MODEL_DESCRIPTION 0x0080
#define RDM_MANUFACTURER_LABEL 0x0081
#define RDM_SOFTWARE_VERSION_LABEL 0x00C0

#define RDM_DMX_PERSONALITY 0x00E0
#define RDM_DMX_PERSONALITY_DESCRIPTION 0x00E1
#define RDM_DMX_START_ADDRESS 0x00F0
#define RDM_SLOT_INFO 0x0120
#define RDM_SLOT_DESCRIPTION 0x121

#define RDM_IDENTIFY_DEVICE 0x1000

//A5 - product category Codes (incomplete)

#define RDM_PRODUCT_CATEGORY_FIXTURE 0x01
#define RDM_PRODUCT_CATEGORY_FIXTURE_FIXED 0x01

//A17 - Response nack reason code defines

#define RDM_NR_UNKNOWN_PID 0x0000
#define RDM_NR_FORMAT_ERROR 0x0001
#define RDM_NR_HARFWARE_FAULT 0x0002
#define RDM_NR_PROXY_REJECT 0x0003
#define RDM_NR_WRITE_PROTECT 0x0004
#define RDM_NR_UNSUPPORTED_COMMAND_CLASS 0x0005
#define RDM_NR_DATA_OUT_OF_RANGE 0x0006
#define RDM_NR_BUFFER_FULL 0x0007
#define RDM_NR_PACKET_SIZE_UNSUPPORTED 0x0008
#define RDM_NR_SUB_DEVICE_OUT_OF_RANGE 0x0009
#define RDM_NR_PROXY_BUFFER_FUL 0x000A


//c1 slot Types (incomplete)

#define RDM_ST_PRIMARY 0x00

//c2 slot definitions (incomplete)

#define RDM_SD_COLOR_ADD_RED 0x0205
#define RDM_SD_COLOR_ADD_GREEN 0x0206
#define RDM_SD_COLOR_ADD_AMBER 0x0211

#define RDM_SD_UNDEFINED 0xFFFF
