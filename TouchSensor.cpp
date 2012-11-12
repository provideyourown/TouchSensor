#include <Arduino.h>
#include "TouchSensor.h"


//unsigned long kDischargeTimeout = 200000UL;
uint16_t kMaxCycles = 500;


byte kSampleDelay = 0;


uint16_t TouchSensor::measure() const
{
  #ifdef INCL_ALTERNATE_METHODS
  return (mUseChargeMethod)? measureViaChargeMethod() : measureViaDischargeMethod();
  #else
  return measureViaChargeMethod();
  #endif
}

void TouchSensor::setSampleSize(byte numSamples)
{
  mStats.setNewSampleSize(numSamples);
}

uint16_t TouchSensor::establishBaseline()
{
  gatherSamples();

  mBaseline = mStats.mean();

  return mBaseline;
}

bool TouchSensor::isTouched()
{
  gatherSamples();

  int measured = mStats.mean();

  int percent = 100 * ( (touchIncreasesTime())? (measured - mBaseline) : (mBaseline - measured) ) / mBaseline;

  return percent > (int)mThreshold;
}

bool TouchSensor::switchPressed()
{
  gatherSamples();
  int measured = mStats.mean();
  return measured >= kMaxCycles; // actually is equal to kMaxCycles+1
}

int TouchSensor::getTouchPercent() const
{
  int measured = mStats.mean();
  int percent = 100 * ( (touchIncreasesTime())? (measured - mBaseline) : (mBaseline - measured) ) / mBaseline;
  return percent;
}

// return averaged number of cycles when isTouched was last called
int TouchSensor::getTouchTime() const
{
  return mStats.mean();
}

// return the std deviation of last isTouched samples
int TouchSensor::getTouchDeviation() const
{
  return mStats.stdDeviation();
}

const IntStatistics& TouchSensor::getStats() const
{
  return mStats;
}


bool TouchSensor::touchIncreasesTime() const
{
  #ifdef INCL_ALTERNATE_METHODS
  return (mUseChargeMethod)? true : false;
  #else
  return true;
  #endif
}

void TouchSensor::gatherSamples()
{
  mStats.reset();
  for (uint16_t i=0; i < mStats.sampleSize(); i++)
  {
    mStats.addData(measure());
    delay(kSampleDelay); // don't get them all at once - draw it out a little
  }
}

uint16_t TouchSensor::measureViaChargeMethod() const
{
  // Get the port, mode, pin & bitmask - faster than using Arduino's funcs
  volatile uint8_t* port = portOutputRegister(digitalPinToPort(mPin));
  volatile uint8_t* ddr = portModeRegister(digitalPinToPort(mPin));
  volatile uint8_t* pin = portInputRegister(digitalPinToPort(mPin));
  byte bitmask = digitalPinToBitMask(mPin);

  // Discharge the pin first by setting it low and output
  *ddr  |= bitmask; // pinmode = OUTPUT (ddr=1)
  *port &= ~(bitmask); // digitalWrite(LOW) (port = 0)

  delay(1); // wait for discharge

  // Make the pin an input and measure the charge time
  *ddr &= ~(bitmask); // pinmode = INPUT (ddr=0)
#ifdef INCL_ALTERNATE_METHODS
  if (mUseIntPullup) // turn on internal pullup
    *port |= bitmask; // digitalWrite(HIGH) (port = 1)
#endif

  uint16_t cycles = 0;
  while (!(*pin & bitmask))
  {
    cycles++; // while pin is low count the CPU cycles
    if (cycles > kMaxCycles) break;
  }

  return cycles;
}

#ifdef INCL_ALTERNATE_METHODS
uint16_t TouchSensor::measureViaDischargeMethod() const
{
  // Get the port, mode, pin & bitmask - faster than using Arduino's funcs
  volatile uint8_t* port = portOutputRegister(digitalPinToPort(mPin));
  volatile uint8_t* ddr = portModeRegister(digitalPinToPort(mPin));
  volatile uint8_t* pin = portInputRegister(digitalPinToPort(mPin));
  byte bitmask = digitalPinToBitMask(mPin);

  // charge up the pin's capacitance - set to output & turn on pullup
  *ddr  |= bitmask; // pinmode = OUTPUT (ddr=1)
  *port |= bitmask; // digitalWrite(HIGH) (port = 1)

  delay(1); // charge it up

  // Isolate the pin to measure discharge
  *ddr &= ~(bitmask); // pinmode = INPUT (ddr=0)
  *port &= ~(bitmask); // digitalWrite(LOW) (port = 0)

  unsigned long startTime = micros();
  unsigned long time = 0;
  while ((*pin & bitmask)) // do nothing
  {
    time = ((long)(micros() - startTime));
    if (time > kDischargeTimeout) break;
  }

  return time / 100; // millisecs / 10
}
#endif
