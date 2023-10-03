#include <HT12D.h>


/*
Constructor
pin is the data input pin of microcontroller to which encoder is connected
frequency (in hz) is the fosc oscilattor frequency of the encoder
fosc can be found out from the ht12e datasheet or using oscilloscope
*/
HT12E::HT12E(){}

/*
Returns 16-bit bitstring with first 4 MSB as error codes and rest as data.
LSB is pin D11 of the encoder. 
*/
uint16_t HT12E::read(uint32_t timeout=HT_TIMEOUT)
{
    uint32_t duration; 
    uint64_t start=millis();
    uint8_t counter;
    uint16_t clockLower=clockPulse*0.8, clockUpper=clockPulse*1.2;
    for (counter = 0; counter < 13; ++counter){
        while (digitalRead(inputPin) == LOW){
            if(millis()-start>timeout)
                return HT_ERROR_TIMEOUT;
        }
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

    start = micros();
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

/*
private function to determine clock frequency of encoder
returns duration of one clock pulse in us (microseconds)
returns 0 if error
returns -1 if timeout
*/
int32_t HT12E::detectClock(uint32_t timeout=HT_TIMEOUT){
    uint32_t duration, max=0, min=0xffffffff, start=millis();
    uint8_t pulses;
    for(pulses=0; pulses<13; ++pulses){
        while (digitalRead(inputPin) == LOW){
            if(millis()-start>timeout)
                return -1;
        }
        duration = pulseIn(inputPin, LOW);
        if(duration>max)
            max=duration;
        if(duration<min)
            min=duration;
    }
    float ratio = max/(float)min;
    if(ratio > 36*0.9 & ratio < 36*1.1){
        return min;
    }
    if(ratio > 18*0.9 & ratio < 18*1.1){
        return min/2;
    }
    return 0;

}

/*
Initializes encoder with specified inputPin and frequency
returns 1 for successfull initialization 
*/
bool HT12E::begin(uint8_t pin, uint32_t frequency){
    inputPin = pin;
    pinMode(inputPin, INPUT);
    data = 0;
    clockPulse = 1e6/frequency;
    return 1;
}


/*
Initializes encoder on specified pin
Auto-detects operating frequency
defaults to 3kHz freq if auto-freq fails and returns 0
returns 1 for successfull determination and initialization
*/
bool HT12E::begin(uint8_t pin){
    inputPin = pin;
    pinMode(inputPin, INPUT);
    data = 0;

    uint32_t clockTot=0;
    uint8_t count=0;
    for(int i=0; i<5; i++){
        uint32_t curr = detectClock();
        if(curr==-1){
            clockTot=0;
            break;
        }
        if(curr==0){
            continue;
        }
        count++;
        clockTot += curr;
    }
    if(clockTot==0){
        Serial.println("ERROR - CLOCK NOT DETERMINED.\nUSING DEFAULT FREQ OF 3kHz");
        clockPulse = 1e6/3000;
        return 0;
    }
    clockPulse = clockTot/count;
    #ifdef HT12D_DEBUG    
        Serial.print("clock determined = "); Serial.print(clockPulse);
    #endif
    return 1;
}