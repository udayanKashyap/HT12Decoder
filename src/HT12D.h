#pragma once
#include <Arduino.h>

#ifndef ht12e_h
#define ht12e_h

// Error Codes
#define HT_ERROR_TIMEOUT 0x1000
#define HT_NO_DATA 0x2000
#define HT_NO_SYNC 0x3000

// Address/Data Pins
#define ADDR_0 0
#define ADDR_1 1
#define ADDR_2 2
#define ADDR_3 3
#define ADDR_4 4
#define ADDR_5 5
#define ADDR_6 6
#define ADDR_7 7
#define DATA_8 8
#define DATA_9 9
#define DATA_10 10
#define DATA_11 11

class HT12E
{
public:
    HT12E(uint8_t pin, uint16_t frequency);
    uint32_t read();
    int8_t readPin(const uint8_t pin);

private:
    uint8_t inputPin;  
    uint16_t data; // this is data;
    uint64_t duration; // pulse duration
    uint16_t clockPulse;
};

#endif
