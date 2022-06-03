#pragma once
#ifndef FACTORENCODER_H
#define FACTORENCODER_H

#include <EncoderTool.h>
using namespace EncoderTool;

class FactorEncoder
{
protected:
  Encoder enc;
  int value;
  int delta = 0;
  int prevEncValue;
  bool periodic = false;
  int _min = INT_MIN;
  int _max = INT_MAX;

public:
  FactorEncoder()
  {
    prevEncValue = enc.getValue();
  }
  void begin(int pinA, int pinB, CountMode countmode = CountMode::quarter, int inputMode = INPUT_PULLUP)
  {
    ASSERT(enc.begin(pinA, pinB, countmode, inputMode));
  }
  void setLimits(int min, int max, bool periodic = false) { 
    this->_min = min;
    this->_max = max;
    this->periodic = periodic;
   }
  bool valueChanged()
  {
    bool changed = enc.valueChanged();
    return changed;
  }
  void setValue(int value) {
      this->value = value;
  }
  int getValue(int factor)
  {
    int newValue = enc.getValue();
    if (newValue != prevEncValue)
    {
      delta = newValue - prevEncValue;
      value += delta * factor;
      prevEncValue = newValue;
      if(periodic) {
        while(value<_min)
          value += (_max - _min);
        while(value>=_max)
          value -= (_max - _min);
      } else {
        value = min(_max, max(value, _min));
      }
    }
    return value;
  }
  float get01Val(int factor) {
      float raw = this->getValue(factor);
          raw -= _min;
          return raw / (float)(_max - _min);
  }
};

#endif