/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

 // Example adapted for WS PiPicoZero
//  PiPico   MCP9601  SSD1306
//
//  +----+   +---+    +---+
//  | 3V3|---|Vin|----|Vin|
//  | gnd|---|gnd|----|gnd|
//  | GP4|---|SDA|----|SDA|
//  | GP5|---|SCL|----|SCL|
//  +----+   +---+    +---+
//

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//*
#include "Adafruit_MCP9601.h"
#define I2C_ADDRESS (0x67)
Adafruit_MCP9601 mcp;
//*


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

const int  debugPin = 0;           //GPIO 0
const int  ControllerOutPin = 1;   //GPIO 1
float SetPoint = 30.0;
float ActualVal = 0.0;
float Controldiff = 0.0;

bool debug_akt = true;

void setup() {
    pinMode(debugPin,INPUT_PULLUP);
    pinMode(ControllerOutPin,OUTPUT);
    debug_akt = !(digitalRead(debugPin));  // debug wenn Pin connected to gnd
    Serial.begin(115200);
    while (!Serial & debug_akt) {
      delay(10);
    }
    if(debug_akt){Serial.println("Adafruit MCP9601 test");}
    /* Initialise the driver with I2C_ADDRESS and the default I2C bus. */
    if (! mcp.begin(I2C_ADDRESS)) {
        if(debug_akt){Serial.println("Sensor not found. Check wiring!");}
        while (1);
    }
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      if(debug_akt){Serial.println(F("SSD1306 allocation failed"));}
      while (1); // Don't proceed, loop forever
    }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);

  if(debug_akt){Serial.println("Found MCP9601!");}
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  if(debug_akt){Serial.print("ADC resolution set to ");}
  switch (mcp.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:
      if(debug_akt){Serial.print("18");}
      break;
    case MCP9600_ADCRESOLUTION_16:
      if(debug_akt){Serial.print("16");}
      break;
    case MCP9600_ADCRESOLUTION_14:
      if(debug_akt){Serial.print("14");}
      break;
    case MCP9600_ADCRESOLUTION_12: 
      if(debug_akt){Serial.print("12");}
      break;
  }
  if(debug_akt){Serial.println(" bits");}

  mcp.setThermocoupleType(MCP9600_TYPE_K);
  if(debug_akt){Serial.print("Thermocouple type set to ");}
  switch (mcp.getThermocoupleType()) {
    case MCP9600_TYPE_K: 
      if(debug_akt){Serial.print("K");} 
      break;
    case MCP9600_TYPE_J:
      if(debug_akt){Serial.print("J");}
      break;
    case MCP9600_TYPE_T:
      if(debug_akt){Serial.print("T");}
      break;
    case MCP9600_TYPE_N:
      if(debug_akt){Serial.print("N");}
      break;
    case MCP9600_TYPE_S:
      if(debug_akt){Serial.print("S");} 
      break;
    case MCP9600_TYPE_E:
      if(debug_akt){Serial.print("E");} 
      break;
    case MCP9600_TYPE_B:
      if(debug_akt){Serial.print("B");}
      break;
    case MCP9600_TYPE_R:
      if(debug_akt){Serial.print("R");}
      break;
  }
  if(debug_akt){Serial.println(" type");}

  mcp.setFilterCoefficient(3);
  if(debug_akt){
    Serial.print("Filter coefficient value set to: ");
    Serial.println(mcp.getFilterCoefficient());
  }

  mcp.setAlertTemperature(1, 30);
  if(debug_akt){
    Serial.print("Alert #1 temperature set to ");
    Serial.println(mcp.getAlertTemperature(1));
  }
  mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp

  mcp.enable(true);

  if(debug_akt){Serial.println(F("------------------------------"));}
}

void loop() {
    uint8_t status = mcp.getStatus();
  if(debug_akt){
    Serial.print("MCP Status: 0x"); 
    Serial.print(status, HEX);  
    Serial.print(": ");
  }
  if (status & MCP9601_STATUS_OPENCIRCUIT) { 
    if(debug_akt){Serial.println("Thermocouple open!"); }
    return; // don't continue, since there's no thermocouple
  }
  if (status & MCP9601_STATUS_SHORTCIRCUIT) { 
    if(debug_akt){Serial.println("Thermocouple shorted to ground!"); }
    return; // don't continue, since the sensor is not working
  }
  if (status & MCP960X_STATUS_ALERT1 & debug_akt) { Serial.print("Alert 1, "); }
  if (status & MCP960X_STATUS_ALERT2 & debug_akt) { Serial.print("Alert 2, "); }
  if (status & MCP960X_STATUS_ALERT3 & debug_akt) { Serial.print("Alert 3, "); }
  if (status & MCP960X_STATUS_ALERT4 & debug_akt) { Serial.print("Alert 4, "); }
  
  
  if(debug_akt){
    Serial.println();
    Serial.print("Hot Junction: "); Serial.println(mcp.readThermocouple());
    Serial.print("Cold Junction: "); Serial.println(mcp.readAmbient());
    Serial.print("ADC: "); Serial.print(mcp.readADC() * 2); Serial.println(" uV");
  }
  delay(1000);
  temperatureControl();
  textdisplay();

}


void temperatureControl(void){
  ActualVal = mcp.readThermocouple();
  Controldiff = SetPoint - ActualVal;
  if (ActualVal < SetPoint){
    //digitalWrite(ControllerOutPin,HIGH);
    analogWrite(ControllerOutPin,Controldiff);
  } 
  else {
    digitalWrite(ControllerOutPin,LOW);
  }  
}

void textdisplay(void) {
  display.clearDisplay();
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println("MCP9001 TC Type K");
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.println(); 
  display.print("TC Hot : "); display.print(mcp.readThermocouple()); display.println("*C");
  display.print("TC Cold: "); display.print(mcp.readAmbient());; display.println("*C");
  display.print("ADC: "); display.print(mcp.readADC() * 2); display.println(" uV");  
  display.print("SetPoint: "); display.print(SetPoint); display.println("*C");
  display.print("ActVal: "); display.print(ActualVal); display.println("*C"); 
  display.print("Controldiff: "); display.print(Controldiff); display.println("*C"); 


  display.display();
}


