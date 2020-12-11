
#include "HT1621.h"

#define PIN_HT1621_BL     10
#define PIN_HT1621_CS     11
#define PIN_HT1621_RW     12
#define PIN_HT1621_DATA   13

struct DisplayConfig {
    uint8_t m4b;   // MSB 4 bits
    uint8_t l4b;   // LSB 4 bits
    bool bitHL;    // High bits or Low bits
};

uint8_t displayMemory[18];  // Display Segments Memory

DisplayConfig digMap[18] = { //High, Low
    {11,10,  true}, { 9, 8,  true}, { 7, 6,  true},  { 5, 4,  true}, { 3, 2,  true}, { 1, 0,  true}, // Row 1
    {11,10, false}, { 9, 8, false}, { 7, 6, false},  { 5, 4, false}, { 3, 2, false}, { 1, 0, false}, // Row 2
    {12,13,  true}, {14,15,  true}, {16,17,  true},  {12,13, false}, {14,15, false}, {16,17, false}  // Row 3
};

uint8_t segments[] = {
    //FGEDABC.
    0b10111110,  // 0
    0b00000110,  // 1
    0b01111100,  // 2
    0b01011110,  // 3
    0b11000110,  // 4
    0b11011010,  // 5
    0b11111010,  // 6
    0b00001110,  // 7
    0b11111110,  // 8
    0b11011110,  // 9
    0b00000000   // [Empty]
};

HT1621 ht(PIN_HT1621_CS, PIN_HT1621_RW, PIN_HT1621_DATA);

void writeDigit(uint8_t d, uint8_t num){
    DisplayConfig df = digMap[d];
    writeSegments(d, segments[num]);
}

void writeSegments(uint8_t d, uint8_t segData){
    DisplayConfig df = digMap[d];
    uint8_t high4Bit = segData >> 4;
    uint8_t low4Bit  = segData & 0b1111;
    
    if( df.bitHL ){
        high4Bit = high4Bit << 4;
        low4Bit  = low4Bit << 4;
        displayMemory[df.m4b] = displayMemory[df.m4b] & 0b00001111;
        displayMemory[df.l4b] = displayMemory[df.l4b] & 0b00001111;
        displayMemory[df.m4b] = displayMemory[df.m4b] | high4Bit;
        displayMemory[df.l4b] = displayMemory[df.l4b] | low4Bit;
    }else{
        displayMemory[df.m4b] = displayMemory[df.m4b] & 0b11110000;
        displayMemory[df.l4b] = displayMemory[df.l4b] & 0b11110000;
        displayMemory[df.m4b] = displayMemory[df.m4b] | high4Bit;
        displayMemory[df.l4b] = displayMemory[df.l4b] | low4Bit;
    }
    
    ht.write(df.m4b, displayMemory[df.m4b]);
    ht.write(df.l4b, displayMemory[df.l4b]);
}


void setup() {
    uint8_t i;
    pinMode(PIN_HT1621_BL, OUTPUT);
    digitalWrite(PIN_HT1621_BL, LOW);

    ht.begin();
    
    ht.sendCommand(HT1621::RC256K);
    ht.sendCommand(HT1621::BIAS_THIRD_4_COM);
    ht.sendCommand(HT1621::SYS_EN);
    ht.sendCommand(HT1621::LCD_ON);

    //Clear display
    for(i=0; i<18; i++){
        displayMemory[i] = 0;
        ht.write(i,0);
    }
    delay(2000);

    digitalWrite(PIN_HT1621_BL, HIGH); //Turn on backlight LED
    
    for(i=10; i<12; i++){
        ht.write(i, 0b00000001);
        delay( 1000 );
        ht.write(i, 0b00000010);
        delay( 1000 );
        ht.write(i, 0b00000100);
        delay( 1000 );
        ht.write(i, 0b00001000);
        delay( 1000 );
        ht.write(i, 0b00010000);
        delay( 1000 );
        ht.write(i, 0b00100000);
        delay( 1000 );
        ht.write(i, 0b01000000);
        delay( 1000 );
        ht.write(i, 0b10000000);
        delay( 1000 );
    }
    digitalWrite(PIN_HT1621_BL, LOW); //Turn off backlight LED
    
    // write all segments (even dp)
    for(i=0; i<18; i++){
        ht.write(i, 0xFF);
        delay( 2000 );
    }
}

void loop() {}
