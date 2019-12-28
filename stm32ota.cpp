
#include "stm32ota.h"



void stm32SendCommand(unsigned char commd) {    // Tested
  Serial.write(commd);
  Serial.write(~commd);
}

unsigned char stm32Erase() {     // Tested
  stm32SendCommand(STM32ERASE);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
  {
    Serial.write(0xFF);
    Serial.write(0x00);
  }
  else return STM32ERR;
  while (!Serial.available());
  return Serial.read();
}

unsigned char stm32Erasen() {     // Tested
  stm32SendCommand(STM32ERASEN);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
  {
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0x00);
  }
  else return STM32ERR;
  while (!Serial.available());
  return Serial.read();
}

// No test yet
unsigned char stm32Run()   {
  stm32SendCommand(STM32RUN);
  while (!Serial.available());
  if (Serial.read() == STM32ACK) {
    stm32Address(STM32STADDR);
    return STM32ACK;
  }
  else
    return STM32ERR;
}

// No test yet
unsigned char stm32Read(unsigned char * rdbuf, unsigned long rdaddress, unsigned char rdlen) {
  stm32SendCommand(STM32RD);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
    stm32Address(rdaddress);
  else return STM32ERR;
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
    stm32SendCommand(rdlen);
  while (!Serial.available());
  size_t getlen = Serial.available();
  Serial.readBytes(rdbuf, getlen);
  return STM32ACK;
}

unsigned char stm32Address(unsigned long addr) {    // Tested
  unsigned char sendaddr[4];
  unsigned char addcheck = 0;
  sendaddr[0] = addr >> 24;
  sendaddr[1] = (addr >> 16) & 0xFF;
  sendaddr[2] = (addr >> 8) & 0xFF;
  sendaddr[3] = addr & 0xFF;
  for (int i = 0; i <= 3; i++) {
    Serial.write(sendaddr[i]);
    addcheck ^= sendaddr[i];
  }
  Serial.write(addcheck);
  while (!Serial.available());
  return Serial.read();
}

unsigned char stm32SendData(unsigned char * data, unsigned char wrlen) {     // Tested
  Serial.write(wrlen);
  for (int i = 0; i <= wrlen; i++) {
    Serial.write(data[i]);
  }
  Serial.write(getChecksum(data, wrlen));
  while (!Serial.available());
  return Serial.read();
}

char stm32Version() {     // Tested
  unsigned char vsbuf[14];
  stm32SendCommand(STM32GET);
  while (!Serial.available());
  vsbuf[0] = Serial.read();
  if (vsbuf[0] != STM32ACK)
    return STM32ERR;
  else {
    Serial.readBytesUntil(STM32ACK, vsbuf, 14);
    return vsbuf[1];
  }
}

/*
GetID 
[ACK][len][Data...][ACK]
Length of [Data] = [len]+1
*/
/*
String stm32GetId() {     // 
  String id="";
  uint8_t tmp,len,i;
  stm32SendCommand(STM32ID);
  while (!Serial.available());
  // read until ACK
  // chip chinh hang ID 2B
  // chip TQ ID 4B
  tmp = Serial.read();// ACK
  if (tmp == STM32ACK) {
    //Serial.readBytesUntil(STM32ACK, sbuf, 4);
    len = Serial.read() + 1;
    if(len>0){
        for(i=0;i<=len;i++)
          if(Serial.available())id += String(Serial.read(),HEX);
    }
  }

  return "NoDect";
}
*/
unsigned char getChecksum( unsigned char * data, unsigned char datalen) {    // Tested
  unsigned char lendata = datalen;
  for (int i = 0; i <= datalen; i++)
    lendata ^= data[i];
  return lendata;
}



