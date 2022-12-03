#pragma once

class Clock 
{
  public:
    Clock() 
    {
      reset();
    }

    /// reset() makes the timer start over counting from 0.0 seconds.
    void reset() 
    {
      unsigned __int64 pf;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
      freq_ = Real(1.0) / (Real)pf;
      QueryPerformanceCounter( (LARGE_INTEGER *)&baseTime_ );
    }

    /// seconds() returns the number of seconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    Real seconds() 
    {
      unsigned __int64 val;
      QueryPerformanceCounter( (LARGE_INTEGER *)&val );
      return (Real)((val - baseTime_) * freq_);
    }

    /// seconds() returns the number of milliseconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    Real milliseconds() 
    {
      return seconds() * 1000.0f;
    }

  private:
    Real freq_;
    unsigned __int64 baseTime_;
};
