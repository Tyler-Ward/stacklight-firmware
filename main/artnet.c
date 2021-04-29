
#include "artnet.h"

#include "string.h"
#include "stdio.h"
#include "esp_log.h"

#include "rdm.h"
#include "settings.h"
#include "version.h"

#include "product_ids.h"

#include "output.h"
#include "indicators.h"

static const char *TAG = "artnet";

const char artnet_id[] = "Art-Net";
int replylen = 0;

uint8_t ipv4_address[4] = {0,0,0,0};
uint8_t mac_address[6] = {0,0,0,0,0,0};

int artnetReplyLen()
{
    return replylen;
}

int process_frame(uint8_t* packet, unsigned int length)
{
    artnet_header_t* packetHeader = (artnet_header_t*)packet;

    //check packet id
    if(memcmp(packetHeader->ID,artnet_id,8)!=0)
        return ARTNET_ACTION_NONE;

    //switch based on oppcode
    switch(packetHeader->OpCode)
    {
        /*
         * DMX data packet
         */
        case Artnet_OpDmx:
        {
            ardnet_dmx_t* dmx = (ardnet_dmx_t*) packet;

            // check the dmx packet has the correct address
            if(dmx->Net!=settingsGetArtnetNet() 
                || dmx->SubUni!=(settingsGetArtnetSubNet()<<4)+settingsGetArtnetUniverse())
            {
                return ARTNET_ACTION_NONE;
            }

            uint16_t dmxIndex = settingsGetDmxAddr() - 1;
            SetOutputsDMX(dmxIndex, dmx->Data);

            return ARTNET_ACTION_NONE;
            break;
        }

        case Artnet_OpPoll:
        {
          //Serial.println("ArtPoll");
          create_artpollReply(packet);
          return ARTNET_ACTION_SEND_REPLY;

          break;
        }

        /*
         * Address configuration packet
         */
        case Artnet_OpAddress:
        {
            artnet_address_t* address = (artnet_address_t*) packet;

            //if the Netswitch high bit is set then change the Net setting
            if((address->NetSwitch&0x80)!=0)
            {
                //set the network and clear the high order bit
                settingsSetArtnetNet(address->NetSwitch&0x7F);
            }
            else if (address->NetSwitch==0x00)
            {
                //a value of x00 means set to the physicial config so set to 0x00
                settingsSetArtnetNet(0x00);
            }
            
            //if the Subswitch high bit is set then change the Net setting
            if((address->SubSwitch&0x80)!=0)
            {
                //set the network and clear the high order bit
                settingsSetArtnetSubNet(address->SubSwitch&0x0F);
            }
            else if (address->SubSwitch==0x00)
            {
                //a value of x00 means set to the physicial config so set to 0x00
                settingsSetArtnetSubNet(0x00);
            }

            // set the address of output 0
            if((address->SwOut[0]&0x80)!=0)
            {
                //set the network and clear the high order bit
                settingsSetArtnetUniverse(address->SwOut[0]&0x0F);
            }
            else if (address->NetSwitch==0x00)
            {
                //a value of x00 means set to the physicial config so set to 0x00
                settingsSetArtnetUniverse(0x00);
            }

            //process locate commands
            if(address->Command==0x02)
            {
                indicatorsSetLocate(0);
            }
            if(address->Command==0x04)
            {
                indicatorsSetLocate(1);
            }

            // nodes should reply with an artPollReply packet
            create_artpollReply(packet);
            return ARTNET_ACTION_SEND_REPLY;

            break;
        }

        case Artnet_OpTodRequest:
        case Artnet_OpTodControl:
        {
            ESP_LOGI(TAG, "RDM Discover");
            create_artTodData(packet);
            return ARTNET_ACTION_SEND_REPLY;
        }

        case Artnet_OpRdm:
        {
            ESP_LOGI(TAG, "RDM packet");

            artnet_rdm_t* artrdm = (artnet_rdm_t *) packet;

            if(artrdm->Net!=settingsGetArtnetNet() 
                || artrdm->Address!=(settingsGetArtnetSubNet()<<4)+settingsGetArtnetUniverse())
            {
                return ARTNET_ACTION_NONE;
            }

            rdm_t* rdm = (rdm_t*) artrdm->RdmPacket;

            int responce_len = processRdm(rdm);
            if(responce_len>0)
            {
                create_artrdm(packet,responce_len);
                return ARTNET_ACTION_SEND_REPLY;
            }

            return ARTNET_ACTION_NONE;
        }

        default:
            ESP_LOGI(TAG, "discarded packet with code: %02x", packetHeader->OpCode);
            return ARTNET_ACTION_NONE;
        break;
    }
}


//build an artnet reply packet
void create_artpollReply(uint8_t* buffer)
{
    //convert buffer into a reply structure
    artnet_poll_reply_t* reply = (artnet_poll_reply_t*)buffer;

    memcpy(reply->ID,artnet_id,8);          // copy artnet id string
    reply->OpCode = Artnet_OpPollReply;     // poll reply packet
    reply->IPAddr[0] = ipv4_address[0];
    reply->IPAddr[1] = ipv4_address[1];
    reply->IPAddr[2] = ipv4_address[2];
    reply->IPAddr[3] = ipv4_address[3];
    reply->port = 0x1936;                   // Port 6454
    reply->VersionInfoHi = (SOFTWARE_REVISION>>8)&0xFF;
    reply->VersionInfoLo = (SOFTWARE_REVISION)&0xFF;
    reply->NetSwitch = settingsGetArtnetNet();
    reply->SubSwitch = settingsGetArtnetSubNet();
    reply->OemHi = (ARTNET_OEM_CODE>>8)&0xff;
    reply->OemLo = (ARTNET_OEM_CODE)&0xff;  // artnet OEM code
    reply->UbeaVersion = 0;                 // no UBEA is avaliable

    uint8_t status = 0;
    if(indicatorsGetLocate())
    {
        status |= 0x40; // Indicators in locate Mode
    }
    else
    {
        status |= 0xC0; // Indicators in normal Mode
    }
    status |= 0x20;     // Port-Address set by network
    status |= 0x02;     // RDM support
    reply->status = status;

    reply->EstaManLo=(ESTA_ID)&0xFF;
    reply->EstaManHi=(ESTA_ID>>8)&0xFF;
    memset(reply->ShortName,0x00,ShortNameLength);
    memset(reply->LongName,0x00,LongNameLength);
    memset(reply->NodeReport,0x00,LongNameLength);
    sprintf(reply->ShortName,"Stack Light");
    sprintf(reply->LongName,"POE Stack Light");
    sprintf(reply->NodeReport,"#0000 [1] Testing");
    reply->NumPortsHi=0;
    reply->NumPortsLo=1;
    reply->PortTypes[0]=0x80;
    reply->PortTypes[1]=0x00;
    reply->PortTypes[2]=0x00;
    reply->PortTypes[3]=0x00;
    memset(reply->GoodInput,0x00,4);
    memset(reply->GoodOutput,0x00,4);
    reply->GoodOutput[0]=0x80;
    memset(reply->SwIn,0x00,4);
    memset(reply->SwOut,0x00,4);
    reply->SwOut[0]=settingsGetArtnetUniverse();
    reply->SwVideo = 0x00;
    reply->SwMacro = 0x00;
    reply->SwRemote = 0x00;
    reply->Spare1 = 0x00;
    reply->Spare2 = 0x00;
    reply->Spare3 = 0x00;
    reply->Style = ARTNET_ST_NODE;
    reply->MacAddress[0] = mac_address[0];
    reply->MacAddress[1] = mac_address[1];
    reply->MacAddress[2] = mac_address[2];
    reply->MacAddress[3] = mac_address[3];
    reply->MacAddress[4] = mac_address[4];
    reply->MacAddress[5] = mac_address[5];
    memcpy(reply->BindIp,reply->IPAddr,4);
    reply->BindIndex = 0x01;

    uint8_t status2 = 0;
    status2 |= 0x01;    //web browser config
    status2 |= 0x02;    //node is DHCP configured
    status2 |= 0x04;    //node is DCHP capable
    status2 |= 0x08;    //supports 15 bit address
    reply->Status2 = status2;

    memset(reply->Filler,0x00,sizeof(reply->Filler));
    replylen=sizeof(artnet_poll_reply_t);

}

//build an artnet reply packet
void create_artTodData(uint8_t* buffer)
{
    //convert buffer into a reply structure
    artnet_tod_data_t* reply = (artnet_tod_data_t*)buffer;

    memcpy(reply->ID,artnet_id,8);          // copy artnet id string
    reply->OpCode = Artnet_OpTodData;     // poll reply packet
    reply->ProtoVerHi=0x00;
    reply->ProtoVerLo=14;
    reply->RdmVer=0x01;
    reply->Port=0x01;
    memset(reply->Spare,0x00,sizeof(reply->Spare));
    reply->BindIndex=0x01;
    reply->Net=settingsGetArtnetNet();
    reply->CommandResponse=0x00;
    reply->Address=(settingsGetArtnetSubNet()<<4)+settingsGetArtnetUniverse();
    reply->UidTotalHi=0x00;
    reply->UidTotalLo=0x01;
    reply->BlockCount=0x00;
    reply->UidCount=0x01;
    memset(reply->Tod,0x00,sizeof(reply->Tod));
    reply->Tod[0][0]=(ESTA_ID>>8)&0xff;
    reply->Tod[0][1]=(ESTA_ID)&0xff;
    reply->Tod[0][2]=0x00;
    reply->Tod[0][3]=0x00;
    reply->Tod[0][4]=0x00;
    reply->Tod[0][5]=0x00;

    replylen=sizeof(artnet_tod_data_t)-6;
}


void create_artrdm(uint8_t* buffer, int rdmlen)
{
    //convert buffer into a reply structure
    artnet_rdm_t* reply = (artnet_rdm_t*)buffer;

    memcpy(reply->ID,artnet_id,8);          // copy artnet id string
    reply->OpCode = Artnet_OpRdm;     // poll reply packet
    reply->ProtoVerHi=0x00;
    reply->ProtoVerLo=14;
    reply->RdmVer=0x01;
    reply->Filler2=0x00;
    memset(reply->Spare,0x00,7);
    reply->Net=settingsGetArtnetNet();
    reply->Command=0x00;
    reply->Address=(settingsGetArtnetSubNet()<<4)+settingsGetArtnetUniverse();
    memcpy(reply->RdmPacket,rdmgetBuffer(),rdmlen);

    replylen=sizeof(artnet_rdm_t)-32+rdmlen;
}

void setIpAddress(uint8_t* address)
{
    memcpy(ipv4_address,address,4);
}

void setMacAddress(uint8_t* address)
{
    memcpy(mac_address,address,6);
}

