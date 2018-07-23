/**
  This example uses the TouchSensor library to sense touch on a metal pad connected to a digital pin.

  For details see the documentation.txt file.
*/

#include <Arduino.h>
#include "../libraries/TouchSensor/TouchSensor.h"

//#define debug

#ifdef debug
  #include "../libraries/Streaming/Streaming.h"
  #include "../libraries/nokiaLCD/nokiaLCD.h"
#endif

//TouchSensor touchChargeInt(4, true, true); // touch sensor is hooked up to digital pin 4
//TouchSensor touchChargeExt(4, true, false); // touch sensor is hooked up to digital pin 4
//TouchSensor touchDischarge(4, false); // touch sensor is hooked up to digital pin 4

TouchSensor  sensor(4);
int kIndicator = 13; // digital pin for LED to indicate touch


#ifdef debug
  // pins are in this order: reset, dc (d/c), sdin (mosi - data in), sclk (sck - clock), sce (chip enable), led (backlight)
  nokiaLCD display(10,11,12,13,7,6);
#endif


bool testTouch(TouchSensor& sensor)
{
  bool isTouched = sensor.isTouched();

  #ifdef debug
  int base =  sensor.getBaseline();
  int cycles = sensor.getTouchTime();
  int deviation = sensor.getTouchDeviation();

  Serial << "sample: " << cycles << endl;
  display.gotoLine(1);
  display << "BaseL: " << base << "(cy)" << endl;
  display.gotoLine(2);
  display << "Value: " << cycles << "(cy)" << endl;
  display.gotoLine(3);
  display << "% Chg: " << 100 * (cycles - base) / base << " % " << endl;
  display.gotoLine(4);
  display << "StdDev: " << deviation << "   " << endl;
  #endif

  return isTouched;
}



//TouchSensor& sensor = touchChargeInt;
//TouchSensor& sensor = touchChargeExt;
//TouchSensor& sensor = touchDischarge;


void setup()
{
#ifdef debug
  Serial.begin(9600);
  Serial << "Starting..." << endl;
  display.init();
  display.backlight(true);
  display.home();
  display << "Ready" << endl;
#endif

  //delay(500);
  sensor.setThreshold(100);
  sensor.setSampleSize(100);

  sensor.establishBaseline();

  pinMode(kIndicator, OUTPUT); // init indicator led
}

void loop()
{
  bool touched = testTouch(sensor);
  if (!touched)
    touched = sensor.switchPressed();

  digitalWrite(kIndicator, touched);

  delay(200);
}


