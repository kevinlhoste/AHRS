#include "MPU9250.h"

MPU9250 mpu;

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(2000);
    mpu.setup();
}

void loop()
{
    static uint32_t prev_ms = millis();
    if ((millis() - prev_ms) > 16)
    {
        mpu.update();
        Serial.print("Raw:");
        int accX=int((int)1000 * mpu.getAcc(0));
        int accY=int((int)1000 * mpu.getAcc(1));
        int accZ=int((int)1000 * mpu.getAcc(2));
        int gX=int(mpu.getGyro(0));
        int gY=int(mpu.getGyro(1));
        int gZ=int(mpu.getGyro(2));
        int mX=int(mpu.getMag(0));
        int mY=int(mpu.getMag(1));
        int mZ=int(mpu.getMag(2));
        Serial.print(accX); Serial.print(",");
        Serial.print(accY); Serial.print(",");
        Serial.print(accZ); Serial.print(",");
        Serial.print(gX); Serial.print(",");
        Serial.print(gY); Serial.print(",");
        Serial.print(gY); Serial.print(",");
        Serial.print(mX); Serial.print(",");
        Serial.print(mY); Serial.print(",");
        Serial.print(mZ); Serial.println("");
        prev_ms = millis();
    }
}
