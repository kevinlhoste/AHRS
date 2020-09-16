#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include "FS.h"
#include "SPIFFS.h"
#include "MPU9250.h"
#include <Adafruit_NeoPixel.h> 

#define PIN           15//gpio12
#define NUMPIXELS     1
float calibrationValues[12];
float heading;
float tiltHeading;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


float mag_hardiron[3]{16.65,35.26,-8.18};
float mag_softiron[9]={0.987,-0.013,-0.011,-0.013,1.013,0.006,-0.011,0.006,1.001};

MPU9250 mpu;

WiFiMulti WiFiMulti;

int opMode=0;

// WiFi network name and password:
const char * ssid = "Air DJ";
const char * password = "";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "150.10.145.150";
const int udpPort = 2390;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void getHeading(void)
{
  heading=(180*atan2(mpu.getMag(1),mpu.getMag(0)))/PI;
  if(heading <0) heading +=360;
}
void getTiltHeading(void)
{
 // float pitch = asin(-Axyz[0]);
 // float roll = asin(Axyz[1]/cos(pitch));

  float xh = mpu.getMag(0) * cos(mpu.getPitch()) + mpu.getMag(2) * sin(mpu.getPitch());
  float yh = mpu.getMag(0) * sin(mpu.getRoll()) * sin(mpu.getPitch()) + mpu.getMag(1) * cos(mpu.getRoll()) - mpu.getMag(2) * sin(mpu.getRoll()) * cos(mpu.getPitch());
  float zh = -mpu.getMag(0) * cos(mpu.getRoll()) * sin(mpu.getPitch()) + mpu.getMag(1) * sin(mpu.getRoll()) + mpu.getMag(2) * cos(mpu.getRoll()) * cos(mpu.getPitch());
  tiltHeading = 180 * atan2(yh, xh)/PI;
  if(yh<0)    tiltHeading +=360;
}

void connect_wifi() {
  // WiFi.disconnect();
  if (WiFi.status() != WL_CONNECTED) { // FIX FOR USING 2.3.0 CORE (only .begin if not connected)
    //WiFi.forceSleepWake();
    WiFi.persistent(false);
    WiFi.setAutoConnect ( false );                                             // Autoconnect to last known Wifi on startup
    WiFi.setAutoReconnect ( false ) ;
    //WiFi.config(ip, gateway, subnet);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); // connect to the network
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void disconnect_wifi() {
  Serial.println("Closing Wifi");
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect();
  //WiFi.forceSleepBegin();
  delay(1);
}

void saveCalibration()
{
    calibrationValues[0]=mpu.getAccBias(0);
    calibrationValues[1]=mpu.getAccBias(1);
    calibrationValues[2]=mpu.getAccBias(2);
    calibrationValues[3]=mpu.getGyroBias(0);
    calibrationValues[4]=mpu.getGyroBias(1);
    calibrationValues[5]=mpu.getGyroBias(2);
    calibrationValues[6]=mpu.getMagBias(0);
    calibrationValues[7]=mpu.getMagBias(1);
    calibrationValues[8]=mpu.getMagBias(2);
    calibrationValues[9]=mpu.getMagScale(0);
    calibrationValues[10]=mpu.getMagScale(1);
    calibrationValues[11]=mpu.getMagScale(2);
  Serial.printf("Writing file");
  File file = SPIFFS.open("/cal.txt", "w+");
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    else{
       float test=10.5;
       for(int i=0;i<=11;i++)
        {
        Serial.println("writing"+String(calibrationValues[i]));
        file.println(calibrationValues[i]);
        }
    }
    file.close();
}


void readCalibration()
{
  Serial.printf("Reading file");
  File file = SPIFFS.open("/cal.txt", "r");
  if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.print("- read from file:");
    int i=0;
    while(file.available()){
         //Lets read line by line from the file
         String line = file.readStringUntil('n');
         Serial.println(line);
         float readFloat=line.toFloat();
         calibrationValues[i]=readFloat;
         /*test2=test2+0.1;
         Serial.println(test2);*/
    }
  file.close();
  mpu.setAccBias(0, calibrationValues[0]);
  mpu.setAccBias(1, calibrationValues[1]);
  mpu.setAccBias(2, calibrationValues[2]);
  mpu.setGyroBias(0,calibrationValues[3]);
  mpu.setGyroBias(1, calibrationValues[4]);
  mpu.setGyroBias(2, calibrationValues[5]);
  mpu.setMagBias(0,calibrationValues[6]);
  mpu.setMagBias(1, calibrationValues[7]);
  mpu.setMagBias(2, calibrationValues[8]);
  mpu.setMagScale(0, calibrationValues[9]);
  mpu.setMagScale(1, calibrationValues[10]);
  mpu.setMagScale(2, calibrationValues[11]);
}



void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
 /* pixels.begin(); // This initializes the NeoPixel library.
  pixels.setPixelColor(0, pixels.Color(100,100,100));
  pixels.show();*/
  delay(500);
  // We start by condelay(500);
  WiFi.mode(WIFI_OFF);
  WiFi.persistent(false);
  WiFi.setAutoConnect ( false );
  WiFi.setAutoReconnect ( false );
  delay(500);
  connect_wifi();
  delay(100);
  if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
  Wire.begin();
  delay(2000);
  mpu.setup(Wire);
  udp.beginPacket(udpAddress,udpPort);
  udp.printf("AccelGyro calibration stay still...");
  udp.endPacket();
  delay(500);
  mpu.calibrateAccelGyro();
  udp.beginPacket(udpAddress,udpPort);
  udp.printf("accelgyro calibration OK!");
  udp.endPacket();
  delay(500);
  udp.beginPacket(udpAddress,udpPort);
  udp.printf("Magnetometer calibration move it move it !");
  udp.endPacket();
  //mpu.calibrateMag();
  udp.beginPacket(udpAddress,udpPort);
  udp.printf("Magnetometer calibration OK !");
  udp.endPacket();
 // saveCalibration();
 // readCalibration();
 
}

void loop(){
  
static uint32_t prev_ms = millis();
    if ((millis() - prev_ms) > 16)
    {
  mpu.update();
  //calibrate 
  float mx = (mpu.getMag(0)/10) - mag_hardiron[0];
  float my = (mpu.getMag(1)/10) - mag_hardiron[1];
  float mz = (mpu.getMag(2)/10) - mag_hardiron[2];
    // soft iron cal
  float cal_mx  = mx * mag_softiron[0] + my * mag_softiron[1] + mz * mag_softiron[2];
  float cal_my =  mx * mag_softiron[3] + my * mag_softiron[4] + mz * mag_softiron[5];
  float cal_mz =  mx * mag_softiron[6] + my * mag_softiron[7] + mz * mag_softiron[8];
  heading=( (180*atan2( (cal_my*10), (cal_mx*10)))/PI);
  if(heading <0) heading +=360;
  Serial.print(cal_mx);
  Serial.print(" ");
  Serial.print(cal_my);
  Serial.print(" ");
  Serial.println(cal_mz);
  Serial.print("heading ");
  Serial.println(heading);

  int intHeading=round(heading);
  /*Serial.print(" tilt: ");
  int intTiltHeading=round(tiltHeading);
  Serial.println(intTiltHeading);*/
  //only send data when connected
    //Send a packet
   udp.beginPacket(udpAddress,udpPort);
    udp.printf("heading %d",   intHeading);
   // udp.printf(" tilHeading %d",   intTiltHeading);
    udp.endPacket();
  
    //  pixels.show();
  //Wait for 1 second
 // delay(10);
    prev_ms = millis();
    }
}
