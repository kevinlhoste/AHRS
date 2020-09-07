#include "FS.h"
#include "SPIFFS.h"
#include "MPU9250.h"
float calibrationValues[12];

MPU9250 mpu;

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

/*void loadCalibration()
{
  Serial.printf("Reading file");
  File file = SPIFFS.open("/cal.txt", "r");
  if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.print("- read from file:");
    while(file.available()){
         //Lets read line by line from the file
         String line = file.readStringUntil('n');
         Serial.println(line);
         float tempFloat=line.toFloat();
         /*test2=test2+0.1;
         Serial.println(test2);*/
   /*      mpu.setAccBias(0, EEPROM.readFloat(EEP_ACC_BIAS + 0));
        mpu.setAccBias(1, EEPROM.readFloat(EEP_ACC_BIAS + 4));
        mpu.setAccBias(2, EEPROM.readFloat(EEP_ACC_BIAS + 8));
        mpu.setGyroBias(0, EEPROM.readFloat(EEP_GYRO_BIAS + 0));
        mpu.setGyroBias(1, EEPROM.readFloat(EEP_GYRO_BIAS + 4));
        mpu.setGyroBias(2, EEPROM.readFloat(EEP_GYRO_BIAS + 8));
        mpu.setMagBias(0, EEPROM.readFloat(EEP_MAG_BIAS + 0));
        mpu.setMagBias(1, EEPROM.readFloat(EEP_MAG_BIAS + 4));
        mpu.setMagBias(2, EEPROM.readFloat(EEP_MAG_BIAS + 8));
        mpu.setMagScale(0, EEPROM.readFloat(EEP_MAG_SCALE + 0));
        mpu.setMagScale(1, EEPROM.readFloat(EEP_MAG_SCALE + 4));
        mpu.setMagScale(2, EEPROM.readFloat(EEP_MAG_SCALE + 8));
    }
  file.close();
}




/*
void saveCalibration()
{
  bool result = SPIFFS.begin(true);
  Serial.print("SPIFFS opened: " + result);
  Serial.println(result);
  File f = SPIFFS.open("/cal.txt", "w+");
  //file is found and not a directory
  if(f && !f.isDirectory())
   {
      Serial.println("file exists!");
       // we could open the file
      while(f.available()) {
         f.println("EEP_CALIB_FLAG");
         f.println("password=123455secret"); 
        }
   }
   else 
      {
      Serial.println("file doesnot exists!");
      File fw = SPIFFS.open("/cal.txt", "w+");
      if(fw && !fw.isDirectory())
        {
        f.println("ssid=abc");
         f.println("password=123455secret"); 
        }
      else Serial.println("Creation failed");
     }
f.close();
}
    
    
 /*   EEPROM.writeByte(EEP_CALIB_FLAG, 1);
    EEPROM.writeFloat(EEP_ACC_BIAS + 0, mpu.getAccBias(0));
    EEPROM.writeFloat(EEP_ACC_BIAS + 4, mpu.getAccBias(1));
    EEPROM.writeFloat(EEP_ACC_BIAS + 8, mpu.getAccBias(2));
    EEPROM.writeFloat(EEP_GYRO_BIAS + 0, mpu.getGyroBias(0));
    EEPROM.writeFloat(EEP_GYRO_BIAS + 4, mpu.getGyroBias(1));
    EEPROM.writeFloat(EEP_GYRO_BIAS + 8, mpu.getGyroBias(2));
    EEPROM.writeFloat(EEP_MAG_BIAS + 0, mpu.getMagBias(0));
    EEPROM.writeFloat(EEP_MAG_BIAS + 4, mpu.getMagBias(1));
    EEPROM.writeFloat(EEP_MAG_BIAS + 8, mpu.getMagBias(2));
    EEPROM.writeFloat(EEP_MAG_SCALE + 0, mpu.getMagScale(0));
    EEPROM.writeFloat(EEP_MAG_SCALE + 4, mpu.getMagScale(1));
    EEPROM.writeFloat(EEP_MAG_SCALE + 8, mpu.getMagScale(2));
    if (b_save) EEPROM.commit();
}*/
/*
void test_write(){
   // always use this to "mount" the filesystem
  bool result = SPIFFS.begin(true);
  Serial.print("SPIFFS opened: " + result);
  Serial.println(result);
  File f = SPIFFS.open("/cal.txt", "r");
   if(f && !f.isDirectory())
   {
      Serial.println("file exists!");
       // we could open the file
      while(f.available()) {
         //Lets read line by line from the file
         String line = f.readStringUntil('n');
         Serial.println(line);
        }
   }
   else 
      {
      Serial.println("file doesnot exists!");
      File fw = SPIFFS.open("/f3.txt", "w+");
      if(fw && !fw.isDirectory())
        {
        Serial.println("Creation suceeded!");
        fw.println("ssid=abc");
        fw.println("password=123455secret"); 
        fw.close(); 
        }
      else Serial.println("Creation failed");
     }
f.close();
}*/


void setup() {
    if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    Serial.begin(115200);
    Wire.begin();
    delay(2000);
    mpu.setup(Wire);
    mpu.calibrateAccelGyro();
    mpu.calibrateMag();
    delay(5000);
    saveCalibration();
    delay(1000);
    readCalibration();

    }

void loop() {

}
