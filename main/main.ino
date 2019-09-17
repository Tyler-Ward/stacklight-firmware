int i;

#include <SPI.h>
#include <Ethernet.h>
#include "artnet.h"

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 17);

EthernetUDP Udp;

void setup() {
  // put your setup code here, to run once:
  analogWrite(3,0);
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(9,0);
  pinMode(13,OUTPUT);

  Ethernet.init(10);
  Ethernet.begin(mac, ip);

  Serial.begin(115200);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Udp.begin(6454);
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

#define ARTNET_BUFFER 530

char packetBuffer[ARTNET_BUFFER];  // buffer to hold incoming packet,

void loop() {

  int packetSize = Udp.parsePacket();
  if(packetSize) {
    Serial.print("Recieved ");
    Serial.println(packetSize);
    Udp.read(packetBuffer, ARTNET_BUFFER);
    int status = process_frame((uint8_t*)packetBuffer, packetSize);
    switch(status)
    {
        case ARTNET_ACTION_NONE:
        break;

        case ARTNET_ACTION_SEND_REPLY:
        byte broadcast[] = {192,168,0,255};
        Udp.beginPacket(broadcast,6454);
        Udp.write(packetBuffer,artnetReplyLen());
        Udp.endPacket();
        break;
    }
  }


  /*EthernetClient client = server.available();
  if(client) {
    Serial.println("new connection");
    analogWrite(3,255);
    client.stop();
  }
  */



  /*
  // put your main code here, to run repeatedly:
  for(i=0;i<256;i++)
  {
    analogWrite(3,i);
    delay(2);
  }
  for(i=255;i>=0;i--)
  {
    analogWrite(3,i);
    delay(2);
  }
  for(i=0;i<256;i++)
  {
    analogWrite(5,i);
    delay(2);
  }
  for(i=255;i>=0;i--)
  {
    analogWrite(5,i);
    delay(2);
  }
  for(i=0;i<256;i++)
  {
    analogWrite(6,i);
    delay(2);
  }
  for(i=255;i>=0;i--)
  {
    analogWrite(6,i);
    delay(2);
  }
  for(i=0;i<256;i++)
  {
    analogWrite(9,i);
    delay(1);
  }
  for(i=255;i>=0;i--)
  {
    analogWrite(9,i);
    delay(1);
  }
  */
}
