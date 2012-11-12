#ifndef BASIC_TEST_H_INCLUDED
#define BASIC_TEST_H_INCLUDED

#include "../TouchSensor.h"

#define debug

#ifdef debug
  #include "../libraries/Streaming/Streaming.h"
  #include "../libraries/nokiaLCD/nokiaLCD.h"
#endif

//TouchSensor touchChargeInt(4, true, true); // touch sensor is hooked up to digital pin 4
//TouchSensor touchChargeExt(4, true, false); // touch sensor is hooked up to digital pin 4
//TouchSensor touchDischarge(4, false); // touch sensor is hooked up to digital pin 4

TouchSensor  sensor(4);
int kIndicator = A1; // digital pin for LED to indicate touch


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
  display.setCursor(1,0);
  display << "BaseL: " << base << "(cy)" << endl;
  display << "Value: " << cycles << "(cy)" << endl;
  display << "% Chg: " << sensor.getTouchPercent() << " % " << endl;
  display << "StdDev: " << deviation << "   " << endl;
  #endif

  return isTouched;
}



//TouchSensor& sensor = touchChargeInt;
//TouchSensor& sensor = touchChargeExt;
//TouchSensor& sensor = touchDischarge;


void touchTestSetup()
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
  sensor.setThreshold(10);
  sensor.setSampleSize(100);

  sensor.establishBaseline();

  pinMode(kIndicator, OUTPUT); // init indicator led
}

void touchTestLoop()
{
  bool touched = testTouch(sensor);
  if (!touched) // check to see if button is pressed
    touched = sensor.switchPressed();
  // light the led if touched
  digitalWrite(kIndicator, touched);

  delay(200);
}


#endif // BASIC_TEST_H_INCLUDED
