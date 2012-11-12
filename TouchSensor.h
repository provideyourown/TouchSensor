
/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 TouchSensor Library
 Author: Scott Daniels
 Website: http://provideyourown.com
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
For more information, see the file documentation.txt that
accompanies this file

Changes:
1.0:
Removed provisions for discharge method & internal pullup
Added test for tact switch
*/

#ifndef TouchSensor_h
#define TouchSensor_h

#include "../libraries/Statistics/IntStatistics.h"

// define INCL_ALTERNATE_METHODS if you wish to include the discharge method or internal pullup in the library
//#define INCL_ALTERNATE_METHODS


class TouchSensor {
public:

#ifdef INCL_ALTERNATE_METHODS
  TouchSensor(byte sensorPin, bool useChargeMethod = true, bool useInternalPullup = true)
    : mPin(sensorPin), mBaseline(0), mThreshold(50), mStats(50), mUseChargeMethod(useChargeMethod), mUseIntPullup(useInternalPullup)
#else
  TouchSensor(byte sensorPin)
    : mPin(sensorPin), mBaseline(0), mThreshold(50), mStats(50)
#endif
    {}

  uint16_t measure() const;

  uint16_t establishBaseline(); // call during setup

  void setThreshold(byte percent) // optional - call during setup; default = 50%
    { mThreshold = percent; }

  void setSampleSize(byte numSamples); // optional - set the number of samples to take for a measurement; default = 50;

  bool isTouched(); // call to test for a touch
  bool switchPressed();

  // utility methods
  int getBaseline() const { return mBaseline; }
  int getTouchPercent() const;
  int getTouchTime() const; // return averaged number of cycles when isTouched was last called
  int getTouchDeviation() const; // return the std deviation of last isTouched samples
  const IntStatistics& getStats() const;

protected:
  byte mPin;
#ifdef INCL_ALTERNATE_METHODS
  bool mUseChargeMethod;
  bool mUseIntPullup;
#endif
  int mBaseline;
  int mThreshold;
  IntStatistics mStats;

  bool touchIncreasesTime() const; // if a touch increases the measure time, return true

  uint16_t measureViaChargeMethod() const;
#ifdef INCL_ALTERNATE_METHODS
  uint16_t measureViaDischargeMethod() const;
#endif

  void gatherSamples();
};


#endif

