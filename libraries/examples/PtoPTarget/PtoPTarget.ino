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
#include <GSM.h>

#if ARDUINO > 18
#include <SPI.h> // needed for Arduino versions later than 0018
#endif

// ==== GSM 
// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "bluevia.movistar.es"
#define GPRS_LOGIN     ""
#define GPRS_PASSWORD  ""

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess(0); // include a 'true' parameter for debug enabled

//char server[] = "instant-dev.herokuapp.com";
//char path[] = "/";
//char server[] = "goo.gl";
//char path[] = "/xkGuzt";
char server[] = "gist.github.com";
char path[] = "/Workman/f859afce9c17aed22356/raw/ab5d258f11982cfa092f6501596b70298ed0f98e/gistfile1.txt";

int port = 80;
// ==== GSM end 


#define NFC_DEMO_DEBUG 1
#define PN532_CS 10
PN532 nfc(PN532_CS);

char last_message[32] PROGMEM = ""; 
String last_messages = ""; 

char DataOut[]="Got it!"; //16bytes
char DataIn[16];//Should be 16bytes
char buffer[32];

// connection state
boolean notConnected = true;  

void setup(void) {
#ifdef NFC_DEMO_DEBUG
  Serial.begin(9600);
  Serial.println("Hello!");
#endif

  //setTime(0,0,0,2,6,2013); // Set a test time

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

  Serial.println("Thingie can touch things!");
}

void loop(void) {
  
  if(nfc.configurePeerAsTarget()) {
    char thetime[20];
    String bufferr;
    //sprintf( thetime, "%d-%d-%d %d:%d:%d", year(), month(), day(), hour(), minute(), second() );
    bufferr = String(thetime);
    last_messages = bufferr;
    Serial.print( "Time:" );
    Serial.println( thetime );
    
    //trans-receive data
    if(nfc.targetTxRx(DataOut,DataIn)) {
      //prog_uchar last_messages[] = "Received!";

      Serial.print("Sending a request");
      
#ifdef NFC_DEMO_DEBUG
      void digitalClockDisplay();
      Serial.print("Reid would you like a.. ");
      Serial.println(DataIn);
#endif

      // if you get a connection, report back via serial:
      if (client.connect(server, port))
      {
        Serial.println("connected");
        // Make a HTTP request:
        client.print("GET ");
        client.print(path);
        client.println(" HTTP/1.0");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println("User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X; de-de) AppleWebKit/523.10.3 (KHTML, like Gecko) Version/3.0.4 Safari/523.10");
        client.println("Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7");
        client.println("Cache-Control: no-cache");
        client.println("Accept-Language: de,en;q=0.7,en-us;q=0.3");
        client.println();
      } 
      else
      {
        // if you didn't get a connection to the server:
        Serial.println("Request failed :(");
      }
      Serial.println();
      Serial.println("Awaiting Response...");
      
      // if the server's disconnected, stop the client:
//      if (!client.available() && !client.connected())
//      {
//        Serial.println();
//        Serial.println("disconnecting.");
//        client.stop();
//      }
    }
  }
  
  while(client.available()){
    char c = client.read();
    Serial.print(c);
  }
  
  //  if( strlen(last_messages) > 0 ){
  //    Serial.print("Last Tap:");
  //    Serial.println( last_messages );
  //    delay(1500);
  //  }



}

int connectGSM()
{
  // Start GSM shield
  while(notConnected)
  {
    Serial.print(".");
    if( (gsmAccess.begin(PINNUMBER)==GSM_READY) & (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY) ){
      notConnected = false;
      Serial.println("Connected!");
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
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

