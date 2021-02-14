# AHRS

Testing AHRS on Movuino

<b>Library used</b>
https://github.com/hideakitai/MPU9250

<b>Calibration software</b>

We need to use this software for getting good calibration data : https://www.pjrc.com/store/prop_shield.html
( source : https://github.com/PaulStoffregen/MotionCal)

using library from https://github.com/hideakitai

tutorial calibration : https://learn.adafruit.com/adafruit-sensorlab-magnetometer-calibration/magnetic-calibration-with-motioncal

<b>Calibration procedure</b>

<u>Option 1:</u> Simple using hide Akitai Library
it uses this algorithm :
https://github.com/kriswiner/MPU6050/wiki/Simple-and-Effective-Magnetometer-Calibration

<u>Option2 : Using MotionCal software from pjrc</u>
1- Install MotionCal software
2- Load sketch motionCal-Movuino
3- record calibration data
problem : how to put matirx of 9 soft iron into library

hard iron correction = magBias[0-3]

<b>Calibration values</b>
MPU9250 bias
x y z  
-66 39 0 mg
2.9 0.6 -0.1 o/s
Mag Calibration: Wave device in a figure eight until done!
mag x min/max:
305
-88
mag y min/max:
465
-19
mag z min/max:
88
-446
Mag Calibration done!
AK8963 mag biases (mG)
190.40, 394.44, -302.99
AK8963 mag scale (mG)
1.20, 0.97, 0.88
< calibration parameters >
accel bias [g]:
-66.71, 39.79, 0.79
gyro bias [deg/s]:
2.86, 0.63, -0.08
mag bias [mG]:
190.40, 394.44, -302.99
mag scale []:
1.20, 0.97, 0.88
