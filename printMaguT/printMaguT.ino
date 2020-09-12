#include "MPU9250.h"

MPU9250 mpu;
long timer1;
float m[3];

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(2000);
    mpu.setup();
    delay(5000);
    mpu.c
    
}

void loop()
{
 static uint32_t prev_ms = millis();
    if ((millis() - prev_ms) > 16)
    {
        mpu.update();
        //mpu.printMagDataT();
        m[0]= mpu.getMag(0)/10;
        m[1]= mpu.getMag(1)/10;
        m[2]= mpu.getMag(2)/10;
        Serial.print(m[0]);
        Serial.print(" ");
        Serial.print(m[1]);
        Serial.print(" ");
        Serial.println(m[2]);
/*
        Serial.print("roll  (x-forward (north)) : ");
        Serial.println(mpu.getRoll());
        Serial.print("pitch (y-right (east))    : ");
        Serial.println(mpu.getPitch());
        Serial.print("yaw   (z-down (down))     : ");
        Serial.println(mpu.getYaw());*/

        prev_ms = millis();
    }
}
