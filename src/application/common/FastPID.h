#ifndef FastPID_H
#define FastPID_H

#include <stdint.h>

#define INTEG_MAX    (INT32_MAX)
#define INTEG_MIN    (INT32_MIN)
#define DERIV_MAX    (INT16_MAX)
#define DERIV_MIN    (INT16_MIN)

#define PARAM_SHIFT  8
#define PARAM_BITS   16
#define PARAM_MAX    (((0x1ULL << PARAM_BITS)-1) >> PARAM_SHIFT) 
#define PARAM_MULT   (((0x1ULL << PARAM_BITS)) >> (PARAM_BITS - PARAM_SHIFT)) 

/*
  A fixed point PID controller with a 32-bit internal calculation pipeline.
*/
class FastPID {

public:
  FastPID() 
  {
    Clear();
  }

  FastPID(float kp, float ki, float kd, float hz, int bits=16, bool sign=false)
  {
    Configure(kp, ki, kd, hz, bits, sign);
  }

  ~FastPID();

  bool SetCoefficients(float kp, float ki, float kd, float hz);
  bool SetOutputConfig(int bits, bool sign);
  bool SetOutputRange(int16_t min, int16_t max);
  void Clear();
  bool Configure(float kp, float ki, float kd, float hz, int bits=16, bool sign=false);
  int16_t Step(int16_t setpoint, int16_t feedback);


private:

  uint32_t FloatToParam(float); 

private:

  // Configuration
  uint32_t ProportionalValue, IntegralValue, DifferentialValue;
  int64_t Outmax, Outmin; 
  
  // State
  int16_t LastSetpoint;
  int64_t Sum;
  int32_t LastError;
};

#endif
