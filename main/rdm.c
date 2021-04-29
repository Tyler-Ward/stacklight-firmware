
#include "rdm.h"
#include "settings.h"
#include "version.h"
#include "stdio.h"
#include "esp_log.h"
#include <string.h>

#include "product_ids.h"

#include "indicators.h"

uint8_t rdm_responce_buffer[52];
bool broadcast=false;

static const char *TAG = "rdm_proc";

//flips the order of the butes in a 16 bit value
static inline uint16_t flipbyteorder(uint16_t pid)
{
    return ((pid>>8)&0xFF) + ((pid&0xFF)<<8);
}

uint8_t* rdmgetBuffer()
{
    return rdm_responce_buffer;
}

int processRdm(rdm_t* rdmin)
{
    //ignore any message outside of start code 0x01
    if(rdmin->subStartCode!=RDM_SC_SUB_MESSAGE)
    {
        ESP_LOGW(TAG, "WRONG START CODE");
        return(0);
    }
    rdm_sub_message_t* rdm = (rdm_sub_message_t *) rdmin;
    //todo check Checksum

    //check for broadcast
    uint8_t addr[]={(ESTA_ID>>8)&0xff,(ESTA_ID)&0xff,0x00,0x00,0x00,0x00};
    uint8_t addr_broadcast[]={0xff,0xff,0xff,0xff,0xff,0xff};
    uint8_t addr_manufacturer_broadcast[]={(ESTA_ID>>8)&0xff,(ESTA_ID)&0xff,0xff,0xff,0xff,0xff};
    broadcast=false;
    if(memcmp(rdm->destination,addr,6))
    {
        //not for us
        if(memcmp(rdm->destination,addr_broadcast,6)==0 || memcmp(rdm->destination,addr_manufacturer_broadcast,6)==0)
        {
            broadcast=true;
        }
        else
        {
            ESP_LOGW(TAG, "WRONG ADDR");
            return(0);
        }
    }

    //if a sub device is addressed then return an nack
    if(rdm->subDevice!=0x0000)
    {
        ESP_LOGI(TAG, "pid : %04x",rdm->subDevice);
        return(rdm_generate_nack_reason(rdm, RDM_NR_SUB_DEVICE_OUT_OF_RANGE));
    }

    uint16_t parameterID = flipbyteorder(rdm->parameterID);

    ESP_LOGI(TAG, "pid : %04x",parameterID);

    if(parameterID == RDM_SUPPORTED_PARAMETERS)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }

        ESP_LOGI(TAG, "GET_SUPORTED_PARAMS");

        //raise a format error nack if data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24+6;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_SUPPORTED_PARAMETERS);
        rdmout->parameterDataLength=10;
        //responce
        //model_description
        rdmout->parameterData[0]=(RDM_DEVICE_MODEL_DESCRIPTION>>8)&0xFF;
        rdmout->parameterData[1]=RDM_DEVICE_MODEL_DESCRIPTION&0xFF;
        //manufacturer_label
        rdmout->parameterData[2]=(RDM_MANUFACTURER_LABEL>>8)&0xFF;
        rdmout->parameterData[3]=RDM_MANUFACTURER_LABEL&0xFF;
        //personality desc
        rdmout->parameterData[4]=(RDM_DMX_PERSONALITY>>8)&0xFF;
        rdmout->parameterData[5]=RDM_DMX_PERSONALITY&0xFF;
        //personality desc
        rdmout->parameterData[6]=(RDM_DMX_PERSONALITY_DESCRIPTION>>8)&0xFF;
        rdmout->parameterData[7]=RDM_DMX_PERSONALITY_DESCRIPTION&0xFF;
        //slot info`
        rdmout->parameterData[8]=(RDM_SLOT_INFO>>8)&0xFF;
        rdmout->parameterData[9]=RDM_SLOT_INFO&0xFF;


        return(finalisePacket(rdmout));
    }

    if(parameterID == RDM_DEVICE_INFO)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }

        ESP_LOGI(TAG, "GET_DEVICE_INFO");

        //raise a format error nack if data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->MessageLength=24+19;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_DEVICE_INFO);
        rdmout->parameterDataLength=0x13;

        //responce
        //rdm ver;
        rdmout->parameterData[0]=0x01;
        rdmout->parameterData[1]=0x00;
        //model ID
        rdmout->parameterData[2]=0x00;
        rdmout->parameterData[3]=0x00;
        //product category
        rdmout->parameterData[4]=RDM_PRODUCT_CATEGORY_FIXTURE;
        rdmout->parameterData[5]=RDM_PRODUCT_CATEGORY_FIXTURE_FIXED;
        //firmware Version
        rdmout->parameterData[6]=0x00;
        rdmout->parameterData[7]=0x00;
        rdmout->parameterData[8]=(SOFTWARE_REVISION>>8)&0xFF;
        rdmout->parameterData[9]=(SOFTWARE_REVISION)&0xFF;
        //footprint
        rdmout->parameterData[10]=0x00;
        rdmout->parameterData[11]=0x04;
        //personality
        rdmout->parameterData[12]=0x01;
        rdmout->parameterData[13]=0x01;
        //start address
        uint16_t dmxAddr=settingsGetDmxAddr();
        rdmout->parameterData[14]=(dmxAddr>>8)&0xFF;
        rdmout->parameterData[15]=dmxAddr&0xFF;
        //sub deveices
        rdmout->parameterData[16]=0x00;
        rdmout->parameterData[17]=0x00;
        //sensor count
        rdmout->parameterData[18]=0x00;

        return(finalisePacket(rdmout));
    }

    if(parameterID == RDM_DEVICE_MODEL_DESCRIPTION)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }

        ESP_LOGI(TAG, "model description");

        //raise a format error nack if data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_DEVICE_MODEL_DESCRIPTION);
        int namelen = sprintf((char*)rdmout->parameterData,"PoE Stack Light");
        rdmout->parameterDataLength=namelen;
        return(finalisePacket(rdmout));

    }

    if(parameterID == RDM_MANUFACTURER_LABEL)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }
        ESP_LOGI(TAG, "Manufacturer label");

        //raise a format error nack if data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_MANUFACTURER_LABEL);
        int namelen = sprintf((char*)rdmout->parameterData,"Tyler Ward");
        rdmout->parameterDataLength=namelen;
        return(finalisePacket(rdmout));

    }

    if(parameterID == RDM_SOFTWARE_VERSION_LABEL)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }
        ESP_LOGI(TAG, "software_version_label");

        //raise a format error nack if data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_SOFTWARE_VERSION_LABEL);
        int namelen = sprintf((char*)rdmout->parameterData, SOFTWARE_VERSION_STRING);
        rdmout->parameterDataLength=namelen;
        return(finalisePacket(rdmout));

    }

    if(parameterID == RDM_DMX_PERSONALITY) //get/set personality
    {
        //we still need to have a set function even though we only have one value
        if(rdm->commandClass == RDM_SET_COMMAND)
        {
            //raise a format error nack if data is too long
            if(rdm->parameterDataLength!=0x01)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            //check range of dmx address
            if(rdm->parameterData[0]!=0x01)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_DATA_OUT_OF_RANGE));
            }

            //confirm write
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->MessageLength=24;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_SET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_DMX_PERSONALITY);
            rdmout->parameterDataLength=0;
            return(finalisePacket(rdmout));
        }
        else
        {
            //raise a format error nack if data is provided
            if(rdm->parameterDataLength!=0x00)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            //reply with address
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->messageLength=26;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_DMX_PERSONALITY);
            rdmout->parameterDataLength=0x02;
            rdmout->parameterData[0]=0x01;
            rdmout->parameterData[1]=0x01;
            return(finalisePacket(rdmout));
        }
    }

    if(parameterID == RDM_DMX_PERSONALITY_DESCRIPTION)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }
        ESP_LOGI(TAG, "personality desc");

        //raise a format error nack if no data is provided
        if(rdm->parameterDataLength!=0x01)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //only have personality 1 so dont return others
        if(rdm->parameterData[0]!=0x01)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_DATA_OUT_OF_RANGE));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24+19;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_DMX_PERSONALITY_DESCRIPTION);
        rdmout->parameterData[0]=0x01;
        rdmout->parameterData[1]=0x00;
        rdmout->parameterData[2]=0x04;

        int namelen = sprintf((char*) &rdmout->parameterData[3],"Default 4ch");
        rdmout->parameterDataLength=3+namelen;

        return(finalisePacket(rdmout));

    }

    if(parameterID == RDM_DMX_START_ADDRESS) //get/set DMX512 addr
    {
        //if a set command then save data
        if(rdm->commandClass == RDM_SET_COMMAND)
        {
            //raise a format error nack if data is too long
            if(rdm->parameterDataLength!=0x02)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            uint16_t recievedDmxAddr=(rdm->parameterData[0]<<8) + rdm->parameterData[1];

            //check range of dmx address
            if(recievedDmxAddr==0 || recievedDmxAddr>512)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_DATA_OUT_OF_RANGE));
            }

            settingsSetDmxAddr(recievedDmxAddr);

            //confirm write
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->MessageLength=24;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_SET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_DMX_START_ADDRESS);
            rdmout->parameterDataLength=0;
            return(finalisePacket(rdmout));
        }
        else
        {
            //raise a format error nack if data is provided
            if(rdm->parameterDataLength!=0x00)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            //reply with address
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->messageLength=26;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_DMX_START_ADDRESS);
            rdmout->parameterDataLength=0x02;
            uint16_t dmxAddr=settingsGetDmxAddr();
            rdmout->parameterData[0]=(dmxAddr>>8)&0xFF;
            rdmout->parameterData[1]=dmxAddr&0xFF;
            return(finalisePacket(rdmout));
        }
    }

    if(parameterID == RDM_SLOT_INFO)
    {
        //this variable cant be set
        if(rdm->commandClass==RDM_SET_COMMAND)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_UNSUPPORTED_COMMAND_CLASS));
        }
        ESP_LOGI(TAG, "slot info");

        //raise a format error nack if no data is provided
        if(rdm->parameterDataLength!=0x00)
        {
            return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
        }

        //Frame
        rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

        rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
        //rdmout->messageLength=24+19;
        memcpy(rdmout->destination,rdm->source,6);
        memcpy(rdmout->source,rdm->destination,6);
        rdmout->transactionNumber=rdm->transactionNumber;
        rdmout->portID=RDM_RESPONSE_TYPE_ACK;
        rdmout->messageCount=0x00;
        rdmout->subDevice=0x00;
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
        rdmout->parameterID=flipbyteorder(RDM_SLOT_INFO);
        rdmout->parameterDataLength=20;
        //slot 1
        rdmout->parameterData[0]=0x00;
        rdmout->parameterData[1]=0x00;
        rdmout->parameterData[2]=RDM_ST_PRIMARY;
        rdmout->parameterData[3]=(RDM_SD_COLOR_ADD_RED>>8)&0xFF;
        rdmout->parameterData[4]=RDM_SD_COLOR_ADD_RED&0xFF;
        //slot 2
        rdmout->parameterData[5]=0x00;
        rdmout->parameterData[6]=0x01;
        rdmout->parameterData[7]=RDM_ST_PRIMARY;
        rdmout->parameterData[8]=(RDM_SD_COLOR_ADD_AMBER>>8)&0xFF;
        rdmout->parameterData[9]=RDM_SD_COLOR_ADD_AMBER&0xFF;
        //slot 3
        rdmout->parameterData[10]=0x00;
        rdmout->parameterData[11]=0x02;
        rdmout->parameterData[12]=RDM_ST_PRIMARY;
        rdmout->parameterData[13]=(RDM_SD_COLOR_ADD_GREEN>>8)&0xFF;
        rdmout->parameterData[14]=RDM_SD_COLOR_ADD_GREEN&0xFF;
        //slot 4
        rdmout->parameterData[15]=0x00;
        rdmout->parameterData[16]=0x03;
        rdmout->parameterData[17]=RDM_ST_PRIMARY;
        rdmout->parameterData[18]=(RDM_SD_UNDEFINED>>8)&0xFF;
        rdmout->parameterData[19]=RDM_SD_UNDEFINED&0xFF;

        return(finalisePacket(rdmout));

    }

    if(parameterID == RDM_IDENTIFY_DEVICE) //get/set identify mode
    {
        //if a set command then save data
        if(rdm->commandClass == RDM_SET_COMMAND)
        {
            //raise a format error nack if data is too long
            if(rdm->parameterDataLength!=0x01)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            //check range of identify input
            if(rdm->parameterData[0]>0x01)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_DATA_OUT_OF_RANGE));
            }

            if(rdm->parameterData[0]==1)
            {
                ESP_LOGI(TAG, "identify on");
                indicatorsSetLocate(1);
            }
            else
            {
                ESP_LOGI(TAG, "identify off");
                indicatorsSetLocate(0);
            }

            //confirm write
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->MessageLength=24;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_SET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_IDENTIFY_DEVICE);
            rdmout->parameterDataLength=0;
            return(finalisePacket(rdmout));
        }
        else
        {
            //raise a format error nack if data is provided
            if(rdm->parameterDataLength!=0x00)
            {
                return(rdm_generate_nack_reason(rdm,RDM_NR_FORMAT_ERROR));
            }

            //reply with identify state
            rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

            rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
            //rdmout->messageLength=26;
            memcpy(rdmout->destination,rdm->source,6);
            memcpy(rdmout->source,rdm->destination,6);
            rdmout->transactionNumber=rdm->transactionNumber;
            rdmout->portID=RDM_RESPONSE_TYPE_ACK;
            rdmout->messageCount=0x00;
            rdmout->subDevice=0x00;
            rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
            rdmout->parameterID=flipbyteorder(RDM_IDENTIFY_DEVICE);
            rdmout->parameterDataLength=0x01;
            if(indicatorsGetLocate())
            {
                //locate enabled
                rdmout->parameterData[0]=1;
            }
            else
            {
                //locate disabled
                rdmout->parameterData[0]=0;
            }
            return(finalisePacket(rdmout));
        }
    }

    ESP_LOGW(TAG, "UNKNOWN PID");


    //send a nack
    return(rdm_generate_nack_reason(rdm, RDM_NR_UNKNOWN_PID));
}

int rdm_generate_nack_reason(rdm_sub_message_t* recieved, uint16_t reason)
{
    rdm_sub_message_t* rdmout = (rdm_sub_message_t *) rdm_responce_buffer;

    rdmout->subStartCode=RDM_SC_SUB_MESSAGE;
    //rdmout->messageLength=26;
    memcpy(rdmout->destination,recieved->source,6);
    memcpy(rdmout->source,recieved->destination,6);
    rdmout->transactionNumber=recieved->transactionNumber;
    rdmout->portID=RDM_RESPONSE_TYPE_NACK_REASON;
    rdmout->messageCount=0x00;
    rdmout->subDevice=0x00;
    if(recieved->commandClass == RDM_GET_COMMAND)
    {
        rdmout->commandClass=RDM_GET_COMMAND_RESPONSE;
    }
    else
    {
        rdmout->commandClass=RDM_SET_COMMAND_RESPONSE;
    }
    rdmout->parameterID=recieved->parameterID;
    rdmout->parameterDataLength=0x02; //two data bits
    rdmout->parameterData[0]=(reason>>8)&0xFF; //high byte
    rdmout->parameterData[1]=reason&0xFF; //low byte
    return(finalisePacket(rdmout));

}

uint8_t finalisePacket(rdm_sub_message_t* packet)
{
    if(broadcast)
        return(0);

    uint8_t length = packet->parameterDataLength+24;
    //set length
    packet->messageLength=length;

    //calculate and set checksum
    uint8_t* pbuf = (uint8_t*) packet;
    uint16_t checksum=0x00CC; //add rdm start code
    int i=0;
    for(i=0;i<(length-1);i++)
    {
        checksum+=pbuf[i];
    }
    pbuf[length-1]=(checksum>>8)&0xFF;
    pbuf[length]=checksum&0xFF;
    return(length+1);
}
