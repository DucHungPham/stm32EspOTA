#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <EEPROM.h>
//#include "stm32ota.h"

#define MAX_SRV_CLIENTS 5
#define port  8686

#define _err  255
#define _cliStop  254

#define NRST 2
#define BOOT0 4
#define LED LED_BUILTIN

char* ssid = "SRS_Testx";  // 
char* password = "12345678";  //


IPAddress local_ip(192,168,0,2);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
//tttt
// web server su dung thu vien ESP8266WebServer
// tcp server su dung thu vien wifi cua arduino
// esp ke thua tu wifi
WiFiServer server(port);//ESP8266WebServer
WiFiClient serverClients[MAX_SRV_CLIENTS];
uint32_t clientTimeout[MAX_SRV_CLIENTS];

void setup() {
  delay(100);
  Serial.begin(115200);//9600
  /*
  Serial.begin(9600, SERIAL_8E1);
  //pinMode(LED_BUILTIN, OUTPUT);
pinMode(BOOT0, OUTPUT);
  pinMode(NRST, OUTPUT);
  pinMode(LED, OUTPUT);
  */
  // load setup
  //uint8_t i =0,tmp=0;
  //while(EEPROM.read(i){
    
   // *(ssid + i) = ;
  //i++;
  //}while 
  //Serial.print("val:"); Serial.println(EEPROM.read(10),HEX);
  //WiFi.hostname("relay");
  delay(100);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.begin();
  //Serial.println("Server started");
  digitalWrite(LED_BUILTIN, HIGH);
}

uint8_t IsNewClient(){
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    //Serial.println("Has Client.");
    for(i = 0; i < MAX_SRV_CLIENTS; i++){

      if(clientTimeout[i] < millis() && serverClients[i]){
          serverClients[i].stop();
          //Serial.print("\nClient ");Serial.print(i,DEC);Serial.print("Stop(Timeout)");
        }
      //find free/disconnected slot//spot
      if (!serverClients[i] || !serverClients[i].connected()){ 
        if(serverClients[i]){serverClients[i].stop();
        //Serial.print("\nClient ");Serial.print(i,DEC);Serial.print("Stop(Disconnect)");
        }
        serverClients[i] = server.available();
        clientTimeout[i] = millis()+5000;
        
        //Serial.print("Client ");Serial.print(i,DEC);Serial.print(": ");
        //Serial.println(serverClients[i].remoteIP());
        //clienRep("Hello client");
        return i;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
    //Serial.println("Client reject.");
    return _cliStop;    
  }
  return _err; 
}

void clienRep(String str){
  uint8_t i;
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
                  if (serverClients[i] && serverClients[i].connected()){
                        digitalWrite(LED_BUILTIN, LOW);
                        serverClients[i].print(str);
                        digitalWrite(LED_BUILTIN, HIGH);
                    }
                  } 
  }
// return ID of client in ClientList
uint8_t  IsRequest(){
  uint8_t i;

  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available())      {
        digitalWrite(LED_BUILTIN, LOW);
        //Serial.print("\nClient: ");Serial.println(i,DEC);
        //serverClients[i].setTimeout(200);
        String recCli ="";
        while (serverClients[i].available()>0) {
          char c = serverClients[i].read();
          recCli +=c;          
          }
        clientTimeout[i] = millis()+5000; 
        Serial.print(recCli); //recCli
        //serverClients[i].print(recCli);
                  
        digitalWrite(LED_BUILTIN, HIGH);
        return i;
      }    
    }
  }
  return _err;
}

void sendResponse(WiFiClient clie,String str){
  clie.println(str);
}
String recStr = "";
//String recCli = "";
// client;
void loop() {
  IsNewClient();
 IsRequest();

  if(Serial.available() >0){
    
        while(Serial.available() >0){
          char c =(char)Serial.read();
            recStr += c;
        }
      
        if(recStr.startsWith("*#") && recStr.endsWith("$@"))
        {       
          uint8_t i=0;
          for(i = 0; i < MAX_SRV_CLIENTS; i++){
          if (serverClients[i] && serverClients[i].connected()){
                digitalWrite(LED_BUILTIN, LOW);
                serverClients[i].print(recStr);
                digitalWrite(LED_BUILTIN, HIGH);
            }
          }
          recStr = "";
          
        }     
        
  }
  
  //
  //Serial.println("Resetting ESP");
  //ESP.restart(); //ESP.reset();
      //

}

