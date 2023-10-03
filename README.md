# HT12Decoder
Library to emulate HT12D decoder using Arduino.

## Installation
Can be installed using the standard Arduino Library install procedure.

## Description
- Library emulates an HT12D 12-bit decoder. 
- It receives data from a HT12E 12-bit Parallel-to-Serial encoder.
- Data can be sent using physical encoder IC or using emulated IC.
- Data sent by the encoder and recceived by this library is processed and converted to parallel.
- Address bits are sent as it is and can be used for authentication using custom code.
