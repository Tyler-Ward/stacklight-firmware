
#include <stdint.h>

#define ARTNET_ACTION_NONE 0x00
#define ARTNET_ACTION_SEND_REPLY 0x01

int process_frame(uint8_t* packet, unsigned int length);
void create_artpollReply(uint8_t* buffer);
void create_artTodData(uint8_t* buffer);
void create_artrdm(uint8_t* buffer, int rdmlen);
int artnetReplyLen();

#define ShortNameLength 18
#define LongNameLength 64
#define MaxNumPorts 4

#pragma pack(push, 1)

typedef struct {
  uint8_t ID[8];               // protocol ID = "Art-Net"
  uint16_t OpCode;             // == OpOutput
  uint8_t ProtVerHi;           // 0
  uint8_t ProtVerLo;           // protocol version, set to ProtocolVersion
} artnet_header_t;

typedef struct {
    uint8_t ID[8];               // protocol ID = "Art-Net"
    uint16_t OpCode;             // == OpOutput
    uint8_t ProtVerHi;           // 0
    uint8_t ProtVerLo;           // protocol version, set to ProtocolVersion
    uint8_t Sequence;
    uint8_t Physical;
    uint8_t SubUni;
    uint8_t Net;
    uint8_t LengthHi;
    uint8_t LengthLo;
    uint8_t Data[];
} ardnet_dmx_t;

typedef struct {
  uint8_t ID[8];
  uint16_t OpCode;
  uint8_t IPAddr[4];
  uint16_t port;
  uint8_t VersionInfoHi;
  uint8_t VersionInfoLo;
  uint8_t NetSwitch;
  uint8_t SubSwitch;
  uint8_t OemHi;
  uint8_t OemLo;
  uint8_t UbeaVersion;
  uint8_t status;
  uint8_t EstaManLo;
  uint8_t EstaManHi;
  char ShortName[ShortNameLength];
  char LongName[LongNameLength];
  char NodeReport[LongNameLength];
  uint8_t NumPortsHi;
  uint8_t NumPortsLo;
  uint8_t PortTypes[MaxNumPorts];
  uint8_t GoodInput[MaxNumPorts];
  uint8_t GoodOutput[MaxNumPorts];
  uint8_t SwIn[MaxNumPorts];
  uint8_t SwOut[MaxNumPorts];
  uint8_t SwVideo;
  uint8_t SwMacro;
  uint8_t SwRemote;
  uint8_t Spare1;
  uint8_t Spare2;
  uint8_t Spare3;
  uint8_t Style;
  uint8_t MacAddress[6];
  uint8_t BindIp[4];
  uint8_t BindIndex;
  uint8_t Status2;
  uint8_t Filler[26];
} artnet_poll_reply_t;

typedef struct {
    uint8_t ID[8];
    uint16_t OpCode;
    uint8_t ProtoVerHi;
    uint8_t ProtoVerLow;
    uint8_t Filler[2];
    uint8_t spare[7];
    uint8_t Net;
    uint8_t AdCount;
    uint8_t LowAddress[32];
} artnet_tod_request_t;

typedef struct {
    uint8_t ID[8];
    uint16_t OpCode;
    uint8_t ProtoVerHi;
    uint8_t ProtoVerLo;
    uint8_t RdmVer;
    uint8_t Port;
    uint8_t Spare[6];
    uint8_t BindIndex;
    uint8_t Net;
    uint8_t CommandResponse;
    uint8_t Address;
    uint8_t UidTotalHi;
    uint8_t UidTotalLo;
    uint8_t BlockCount;
    uint8_t UidCount;
    uint8_t Tod[2][6];
} artnet_tod_data_t;

typedef struct {
    uint8_t ID[8];
    uint16_t OpCode;
    uint8_t ProtoVerHi;
    uint8_t ProtoVerLo;
    uint8_t RdmVer;
    uint8_t Filler2;
    uint8_t Spare[7];
    uint8_t Net;
    uint8_t Command;
    uint8_t Address;
    uint8_t RdmPacket[32];
} artnet_rdm_t;

typedef struct {
    uint8_t ID[8];
    uint16_t OpCode;
    uint8_t ProtoVerHi;
    uint8_t ProtoVerLo;
    uint8_t RdmVer;
    uint8_t Filler2;
    uint8_t UID[6];
    uint8_t Spare1;
    uint8_t CommandClass;
    uint16_t ParameterId;
    uint16_t SubDevice;
    uint16_t SubCount;
    uint8_t spare[4];
    uint16_t Data[16];
} artnet_rdm_sub_t;

#pragma pack(pop)

// artnet OpCodes

#define Artnet_OpPoll 0x2000 // Poll
#define Artnet_OpPollReply 0x2100 // Poll reply

#define Artnet_OpDmx 0x5000

#define Artnet_OpTodRequest 0x8000
#define Artnet_OpTodData 0x8100
#define Artnet_OpTodControl 0x8200
#define Artnet_OpRdm 0x8300
#define Artnet_OpRdmSub 0x8400

//table 4 artnet style Codes

#define ARTNET_ST_NODE 0x00
#define ARTNET_ST_CONTROLLER 0x01
#define ARTNET_ST_MEDIA 0x02
#define ARTNET_ST_ROUTE 0x03
#define ARTNET_ST_BACKUP 0x04
#define ARTNET_ST_CONFIG 0x05
#define ARTNET_ST_VISUAL 0x06
