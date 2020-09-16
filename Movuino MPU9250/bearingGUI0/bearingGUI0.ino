#include <MPU9250_asukiaaa.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include<Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED false
#define VERBOSE //comment for removing printf

#define CALIB_SEC 20

//8hz
unsigned int timer0;
uint8_t sensorId;
bool magCalibrated = false;

MPU9250_asukiaaa mySensor(0x69);
float mDirection, mX, mY, mZ;

const int MPU_addr=0x69; // I2C address of the MPU-6050

const char* ssid = "MovuinoESP32-01";
const char* password = "........";

const char command[] = "requestIMU";
float arrayValues[8];
int indexArray=0;
float averageBearing=0;

WebServer server(80);


WebSocketsServer webSocket = WebSocketsServer(81);

void setMagMinMaxAndSetOffset(MPU9250_asukiaaa* sensor, int seconds) {
  unsigned long calibStartAt = millis();
  float magX, magXMin, magXMax, magY, magYMin, magYMax, magZ, magZMin, magZMax;

  sensor->magUpdate();
  magXMin = magXMax = sensor->magX();
  magYMin = magYMax = sensor->magY();
  magZMin = magZMax = sensor->magZ();

  while(millis() - calibStartAt < (unsigned long) seconds * 1000) {
    delay(100);
    sensor->magUpdate();
    magX = sensor->magX();
    magY = sensor->magY();
    magZ = sensor->magZ();
    if (magX > magXMax) magXMax = magX;
    if (magY > magYMax) magYMax = magY;
    if (magZ > magZMax) magZMax = magZ;
    if (magX < magXMin) magXMin = magX;
    if (magY < magYMin) magYMin = magY;
    if (magZ < magZMin) magZMin = magZ;
  }

  sensor->magXOffset = - (magXMax + magXMin) / 2;
  sensor->magYOffset = - (magYMax + magYMin) / 2;
  sensor->magZOffset = - (magZMax + magZMin) / 2;
}

void calibrateMag(){
  float magXMin, magXMax, magYMin, magYMax, magZ, magZMin, magZMax;
  Serial.println("Start scanning values of magnetometer to get offset values.");
  Serial.println("Rotate your device for " + String(CALIB_SEC) + " seconds.");
  setMagMinMaxAndSetOffset(&mySensor, CALIB_SEC);
  Serial.println("Finished setting offset values.");
  magCalibrated=true;
}



void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void connect_wifi(){
    Serial.println();
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  delay(100);
  Serial.println("Set softAPConfig");
  IPAddress Ip(10, 10, 10, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(100);
}
void handleRoot() {
//  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
 // digitalWrite(led, 0);
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // payload = contenu du message envoyé
        // Send the current LED status
    /*    if (movSTATUS) {
          Serial.println("movSTATUS");
          webSocket.sendTXT(num, avance, strlen(avance));
        }
        else {
          webSocket.sendTXT(num, halte, strlen(halte));
          Serial.println("movSTATUS else");
        }  */
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      //received request of datza
      if (strcmp(command, (const char *)payload) == 0) {
        //webSocket.sendBIN(num, payload, length);
      }
      /*
      else if (strcmp(halte, (const char *)payload) == 0) {
      //  writeLED(false);     
         S();  
      }
      else if (strcmp(droite, (const char *)payload) == 0) {  
         D();  
      }      
      else if (strcmp(gauche, (const char *)payload) == 0) {
        G();
      }
      else if (strcmp(recule, (const char *)payload) == 0) {  
         R();  
      }      
      else {
        Serial.println("Unknown command");
      }*/
      // send data to all connected clients
      //webSocket.broadcastTXT(payload, length);  // envoie le message reçu à tous les appareils connectés
    break;
    case WStype_BIN:
    {
     Serial.printf("[%u] get binary length: %u\r\n", num, length);
      /*#ifdef VERBOSE
        Serial.printf("[%u] get binary length: %u\r\n", num, length);
        Serial.println(" ");
        //hexdump(payload, length);
        Serial.println(length);
        Serial.println(payload[0]);
        Serial.println(payload[1]);
        Serial.println(payload[2]);
      #endif*/
      int frameType = payload[0];
      int command = payload[1];
      int speedValue = payload[2];
      //int outputValue = payload[3];      
      //0: frametype (0 command frame avant arrière stop
      //1: command 
      //2 : speed
         
      if(frameType==0){
        //0 : stop
        if(command==0){
          Serial.println("stop");
          //setDCStop();
        }
        //1 : forward
        else if(command==1){
          Serial.print("forward : ");
          Serial.println(speedValue);
          Serial.print(" ");
          int motorSpeed=map(speedValue,0,100,0,4095);
         // setDCForward(motorSpeed);
          Serial.println(motorSpeed);
       
        }
        //2 : backward
        else if(command==2){
          Serial.println("backward : ");
          Serial.print(speedValue);
          Serial.print(" ");
          int motorSpeed=map(speedValue,0,100,0,4095);
         // setDCBackward(motorSpeed);
          Serial.println(motorSpeed);
       
        }
        //3 : left
        else if(command==3){
          Serial.print("left : ");
          Serial.print(speedValue);
          int motorSpeed=map(speedValue,0,100,0,4095);
         // setDCLeft(motorSpeed);
          Serial.println(motorSpeed);
         
        }
        //4 : right
        else if(command==4){
          Serial.print("right : ");
          Serial.print(speedValue);
          Serial.print(" ");
          int motorSpeed=map(speedValue,0,100,0,4095);
         // setDCRight(motorSpeed);
          Serial.println(motorSpeed);
       
        }
         else if(command==5){
          calibrateMag();
         }
        
      }
     
      //motor frame 
      /*if(outputType==0){
         if(outputId==0){
           Serial.print("Left motor ");
           Serial.print(outputValue);
           Serial.print(" % ");
           if (outputDirection) Serial.println("Forward");
           else Serial.println("Reverse");
         }
         else if(outputId==1){
           Serial.print("Right motor ");
           Serial.print(outputValue);
           Serial.println(" %");
           if (outputDirection) Serial.println("Forward");
           else Serial.println("Reverse");
         }
      }*/
      // echo data back to browser
      //webSocket.sendBIN(num, payload, length);
   }
      break;
      
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      //hexdump(payload, length);
      // echo data back to browser
      webSocket.sendBIN(num, payload, length);
      break;

      
    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}
void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
//  digitalWrite(led, 0);
}

void setup(void) {
  /*MPU6050 init*/
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
  Wire.begin();
  /**/
  Serial.begin(115200);
  mySensor.setWire(&Wire);
  while (mySensor.readId(&sensorId) != 0) {
    Serial.println("Cannot find device to read sensorId");
    delay(2000);
  }
  mySensor.beginMag();
  calibrateMag();
  timer0=millis();
  connect_wifi();
  server.serveStatic("/js", SPIFFS, "/js");
   server.serveStatic("/css", SPIFFS, "/css");
     server.serveStatic("/img", SPIFFS, "/img");
        server.serveStatic("/webfonts", SPIFFS, "/webfonts");
                server.serveStatic("/svgs", SPIFFS, "/svgs");
  server.serveStatic("/", SPIFFS, "/index.html");
  
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
 // ticker1.attach_ms(50, onTick); // le ticker s'active toutes les secondes et exécute onTick

}

void loop(void) {
 if(millis()-timer0>=125){
    mySensor.magUpdate();
    mDirection = mySensor.magHorizDirection();
   
    if(indexArray==7) {
      // arrayValues[indexArray]=mDirection;
       averageBearing=averageBearing/8;
       Serial.println("horizontal direction: " + String(averageBearing));
       indexArray=0;
    }
    else {
       //arrayValues[indexArray]=mDirection;
       averageBearing= averageBearing+mDirection;
       indexArray++;
    }
    //Serial.println("horizontal direction: " + String(mDirection));
   // webSocket.sendTXT(0,""+(String)mDirection);
    timer0=millis();
 }
 webSocket.loop();
 server.handleClient();
}
