#include <PN532.h> // NFC
#include <String.h>  // String Class
#include <avr/pgmspace.h>  // Save to PROGMEM
#include <progmem.h>
#include <cstddef> // Null lib
#include <Time.h>  // Datetime
#include <stdio.h>
#include <stdlib.h>
#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

#if ARDUINO > 18
#include <SPI.h> // needed for Arduino versions later than 0018
#endif

#define NFC_DEMO_DEBUG 1
#define PN532_CS 10
PN532 nfc(PN532_CS);

char last_message[32] PROGMEM = ""; 
String last_messages = ""; 

char DataOut[]="Got it!"; //16bytes
char DataIn[16];//Should be 16bytes
char buffer[32];

void setup(void) {
#ifdef NFC_DEMO_DEBUG
  Serial.begin(9600);
  Serial.println("Hello!");
#endif

  setTime(0,0,0,2,6,2013); // Set a test time

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
#ifdef NFC_DEMO_DEBUG
    Serial.print("Didn't find PN53x board");
#endif

    while (1); // halt
  }

#ifdef NFC_DEMO_DEBUG
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.print("Supports "); 
  Serial.println(versiondata & 0xFF, HEX);
  Serial.print("How much space we have? ");
  Serial.println( memoryFree() );
#endif

  // configure board to read RFID tags and cards
  nfc.SAMConfig();
  
}

void loop(void) {
  if(nfc.configurePeerAsTarget()) {
    char thetime[20];
    String bufferr;
    sprintf( thetime, "%d-%d-%d %d:%d:%d", year(), month(), day(), hour(), minute(), second() );
    bufferr = String(thetime);
    last_messages = bufferr;
    Serial.print( "Time:" );
    Serial.println( thetime );
    
    //trans-receive data
    if(nfc.targetTxRx(DataOut,DataIn)) {
      //prog_uchar last_messages[] = "Received!";

#ifdef NFC_DEMO_DEBUG
      void digitalClockDisplay();
      Serial.print("Reid would you like a.. ");
      Serial.println(DataIn);
#endif

    }
  }
  
  if( strlen(last_messages) > 0 ){
    Serial.print("Last Tap:");
    Serial.println( last_messages );
    delay(1500);
  }

}

// variables created by the build process when compiling the sketch
extern int __bss_end;
extern void *__brkval;
// function to return the amount of free RAM
int memoryFree()
{
 int freeValue;
 if ((int)__brkval == 0)
 freeValue = ((int)&freeValue) - ((int)&__bss_end);
 else
 freeValue = ((int)&freeValue) - ((int)__brkval);
 return freeValue;
}

