#ifndef LIDAR_H
#define LIDAR_H

#define firstFlag 0xAA
#define secondFlag 0x00

#include <Arduino.h>

class Lidar {
public:
  
  Lidar();  // Constructor
  void doSomething();
  String createString(byte rxBuffer[2048], int Length);
};

#endif  // lIDAR_H
