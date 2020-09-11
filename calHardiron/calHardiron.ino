#include "MPU9250.h"

MPU9250 mpu;
long timer1;
float m[3];
float min_x, max_x, mid_x;
float min_y, max_y, mid_y;
float min_z, max_z, mid_z;

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(2000);
    mpu.setup();
    delay(5000);
    mpu.update();
    m[0]= mpu.getMag(0)/10;
    m[1]= mpu.getMag(1)/10;
    m[2]= mpu.getMag(2)/10;
    min_x = max_x = m[0];
    min_y = max_y = m[1];
    min_z = max_z = m[2];
    
}

void loop()
{
 static uint32_t prev_ms = millis();
    if ((millis() - prev_ms) > 16)
    {
        mpu.update();
        mpu.printMagDataT();
        float x= mpu.getMag(0)/10;
        float y= mpu.getMag(1)/10;
        float z = mpu.getMag(2)/10;
/*
        Serial.print("roll  (x-forward (north)) : ");
        Serial.println(mpu.getRoll());
        Serial.print("pitch (y-right (east))    : ");
        Serial.println(mpu.getPitch());
        Serial.print("yaw   (z-down (down))     : ");
        Serial.println(mpu.getYaw());*/

        prev_ms = millis();
        Serial.print("Mag: (");
  Serial.print(x); Serial.print(", ");
  Serial.print(y); Serial.print(", ");
  Serial.print(z); Serial.print(")");

  min_x = min(min_x, x);
  min_y = min(min_y, y);
  min_z = min(min_z, z);

  max_x = max(max_x, x);
  max_y = max(max_y, y);
  max_z = max(max_z, z);

  mid_x = (max_x + min_x) / 2;
  mid_y = (max_y + min_y) / 2;
  mid_z = (max_z + min_z) / 2;
  Serial.print(" Hard offset: (");
  Serial.print(mid_x); Serial.print(", ");
  Serial.print(mid_y); Serial.print(", ");
  Serial.print(mid_z); Serial.print(")");  

  Serial.print(" Field: (");
  Serial.print((max_x - min_x)/2); Serial.print(", ");
  Serial.print((max_y - min_y)/2); Serial.print(", ");
  Serial.print((max_z - min_z)/2); Serial.println(")"); 
    }
}
