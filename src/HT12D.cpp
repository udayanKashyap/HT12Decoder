#include <HT12D.h>


/*
Constructor
pin is the data input pin of microcontroller to which encoder is connected
frequency (in hz) is the fosc oscilattor frequency of the encoder
fosc can be found out from the ht12e datasheet or using oscilloscope
*/
HT12E::HT12E(uint8_t pin, uint16_t frequency)
{
    inputPin = pin;
    pinMode(inputPin, INPUT);
    data = 0;
    clockPulse = 1e6/frequency;
}

/*
Returns 16-bit bitstring with first 4 MSB as error codes and rest as data.
LSB is pin D11 of the encoder. 
*/
uint16_t HT12E::read()
{
    uint8_t counter;
    uint16_t clockLower=clockPulse*0.8, clockUpper=clockPulse*1.2;
    for (counter = 0; counter < 13; ++counter){
        while (digitalRead(inputPin) == LOW);
        duration = pulseIn(inputPin, LOW);

        #ifdef HT12D_DEBUG
            Serial.print(duration); Serial.print(" ");
        #endif
        if (duration>( clockLower*36 ) && duration<( clockUpper*36 ))
            break;
    }

    if (counter == 13){
        #ifdef HT12D_DEBUG
            Serial.print(" ERROR Timeout... ")
        #endif
        return HT_ERROR_TIMEOUT;
    }

    unsigned long start = micros();
    while (1){
        if (digitalRead(inputPin) == LOW){
            break;
        }
        if ((micros() - start) > 10 * clockPulse){
            #ifdef HT12D_DEBUG
                Serial.print(" ERROR No Sync... ")
            #endif
            return HT_NO_SYNC;
        }
    }

    for (data = 0, counter = 0; counter<12; ++counter){
        duration = pulseIn(inputPin, HIGH);
        if (duration>clockLower && duration<clockUpper) {
            data = (data << 1) + 1; 
        }
        else if (duration>(2*clockLower) && duration<(2*clockUpper)){
            data = (data << 1);
        }
        else{
            data = 0;
            #ifdef HT12D_DEBUG
                Serial.print(" ERROR No Data... ")
            #endif
            return HT_NO_DATA;
        }
    }
    return (data);
}

/*
Returns the value at the pin of HT12E Chip
Pins start from 0 to 11
returns -1 if pin is out of range
return -2 if ERROR received from HT12E Chip
*/
int8_t HT12E::readPin(const uint8_t pin)
{
    if (pin > 11)
    {
        return -1;
    }
    uint16_t bitsReceived = read();
    if (bitsReceived > 0x0fff)
    {
        return -2;
    }
    uint16_t mask = 0x0800;
    mask = mask >> pin;
    bitsReceived = (bitsReceived & mask) >> (11 - pin);
    return bitsReceived;
}

/*
Returns the data received as an array of bool data type
Return NULL if read Error occurred
Otherwise returns pointer to bool array of size=12 elements
*/
bool * HT12E::readArr(){
    uint16_t data = read();
    if(data>0x0fff)
        return NULL;
    bool * dataArr = new bool[12];
    for(int i=0; i<12; i++){
        dataArr[i] = (data & (0x0800>>i)) >> (11-i);
    }
    return dataArr;
}