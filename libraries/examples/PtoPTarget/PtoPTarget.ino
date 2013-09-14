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

//////////////////////////////////////////

// GPRS
#define PINNUMBER ""
#define GPRS_APN       "bluevia.movistar.es"
#define GPRS_LOGIN     ""
#define GPRS_PASSWORD  ""

// Payment Endpoint
char server[] = "instant-dev.herokuapp.com";
char path[] = "/demo/prototype?transact=credit&source_name=unit_1&target_name=unit_2&amount=1";
int port = 80;

// Device Mode
char mode[] = "RECEIVER"; // SENDER

// NFC Response (Receiver)
char DataOut[]="Receiver: Got it!";

//////////////////////////////////////////

// GPRS Connection
GSMClient client;
GPRS gprs;
GSM gsmAccess(0); // include a 'true' parameter for debug enabled
GSMScanner scannerNetworks;
boolean notConnected = true;  

// NFC
#define NFC_DEMO_DEBUG 1
#define PN532_CS 10
PN532 nfc(PN532_CS);

// NFC Response (Receiver)
char DataIn[16];
char buffer[32];

// LED 
int ledPin = 13;

void setup(void) {
#ifdef NFC_DEMO_DEBUG
  Serial.begin(9600);
  Serial.println("Starting");
#endif
  
  pinMode(ledPin, OUTPUT);

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

  Serial.println("Device Ready!");
  Serial.println("");
}

void loop(void) {
  Serial.print(".");
  if(nfc.configurePeerAsTarget()) {
    
    //trans-receive data
    if(nfc.targetTxRx(DataOut,DataIn)) {
      
#ifdef NFC_DEMO_DEBUG
      void digitalClockDisplay();
      Serial.print("NFC Incoming: ");
      Serial.println(DataIn);
#endif

      sendRequest();
      
      Serial.println("Awaiting Response...");
      Serial.println();
    }
  }
  
  printClientResponse();
  
}



int connectGSM() {
  Serial.println();
  Serial.println("Connecting to GSM network. This may take up to a few minutes...");
  
  scannerNetworks.begin();
  
  // Start GSM shield
  while(notConnected)
  {
    if( (gsmAccess.begin(PINNUMBER)==GSM_READY) & (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY) ){
      notConnected = false;
      Serial.print("GSM connected!");
      statusGSM();
    } else {
      Serial.println("GSM failed to connect. retrying...");
      delay(1000);
    }
  }
  
}

void statusGSM(){
  if( notConnected ){
    Serial.print("Not Connected.");
  } else {
    Serial.print("Carrier: ");
    Serial.print(scannerNetworks.getCurrentCarrier());
    
    Serial.print(" || Signal: ");
    Serial.print(scannerNetworks.getSignalStrength());
    Serial.println(" [0-31]");
  }
}

int disconnectGSM() {
  // if the server's disconnected, stop the client:
  if ( !client.available() && !client.connected() ) {
    Serial.println();
    Serial.println("Disconnecting");
    client.stop();
  }
}

int sendRequest() {
  
  if( notConnected ){
    connectGSM();
  }
  
  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.print("Starting request... ");
    
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.print("Host: ");
    client.println(server);
    client.println();
  } else {
    Serial.println("Request failed");
  }
}

int printClientResponse(){
  while(client.available()){
    char c = client.read();
    Serial.print(c);
  }
}

void modeSender(){
  
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

