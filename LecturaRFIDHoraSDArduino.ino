/**************************************************************************/
/*! 
    @file     readMifare.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.
   
    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:
   
    - Authenticate block 4 (the first block of Sector 1) using
      the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
    - If authentication succeeds, we can then read any of the
      4 blocks in that sector (though only block 4 is read here)
	 
    If the card has a 7-byte UID it is probably a Mifare
    Ultralight card, and the 4 byte pages can be read directly.
    Page 4 is read by default since this is the first 'general-
    purpose' page on the tags.


This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 
These chips use SPI or I2C to communicate.

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include "RTClib.h"
RTC_DS1307 RTC;
SoftwareSerial mySerial(8, 9); // RX, TX

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a software SPI connection (recommended):
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

//SoftwareSerial EdisonSerial(10, 11); // RX, TX
const int chipSelect = 4;
void setup(void) {
  Serial.begin(115200);
  Serial.println("Serial Connection Successul, BD = 115200.");
  mySerial.begin(4800);
  // set the data rate for the SoftwareSerial port
  //EdisonSerial.begin(9600);
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();


  
  Serial.println("Waiting for an ISO14443A Card ...");

  tone(7, 3000, 100);
  delay(100);
  tone(7, 5000, 100);

      Wire.begin();
    RTC.begin();
    RTC.adjust(DateTime(__DATE__, __TIME__)); //Update time
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  else{
    Serial.println("RTC is running!");
  }

    Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)


  // make a string for assembling the data to log:
  String dataString = "";
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
	//Serial.println("*******************************************");
    //Serial.println("Found an ISO14443A card");
    //Serial.print("  UID Length: ");
    //Serial.print(uidLength, DEC);Serial.println(" bytes");
    //Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);

    //Registrar en SD
    for(int i=0; i<uidLength; i++)
    {
      dataString += String(uid[i]);
      dataString += " ";
    }

    dataString += "\tFECHA: ";
      
    DateTime now = RTC.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(); 

    dataString += String(now.year());
    dataString += "/";
    dataString += String(now.month());
    dataString += "/";
    dataString += String(now.day());
    dataString += "  ";

    dataString += String(now.hour());
    dataString += ":";
    dataString += String(now.minute());
    dataString += ":";
    dataString += String(now.second());
    dataString += " ";
     // turn off tone function for pin 7:
    // play a note on pin 7 for 500 ms:
    tone(7, 3000, 50);



    if(mySerial.isListening())
    {
      if(uidLength == 4)
      {
        for(int i=0; i<4; i++)
        {
          mySerial.write(uid[i]);
        }

        mySerial.write(255);
        mySerial.write(255);
        mySerial.write(255);
        
      }
      else
      {
        for(int i=0; i<7; i++)
        {
          mySerial.write(uid[i]);

        }
      
      }

    }

      // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("Registro.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening Registro.txt");
  }

    tone(7, 5000, 100);
 /*   
    if (EdisonSerial.available()) 
    {
      EdisonSerial.write(uid[0]);
    }
    
  */
    //Serial.println("*******************************************\n");
	
  }
}

