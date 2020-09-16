#include "MPU9250.h"

MPU9250 mpu;

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(2000);
    mpu.setup();
    delay(5000);
    Serial.println("selftest()");
    mpu.selfTest();
}

void loop()
{
     
delay(1000);
}
