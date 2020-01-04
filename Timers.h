#ifndef HEADER_TIMERCLASS
  #define HEADER_TIMERCLASS
  #include <Wire.h>

  #include "Sodaq_DS3231.h"
  
  #include <Adafruit_SSD1306.h>
  #include <Adafruit_GFX.h>
  
  #include <idDHT11.h>
  #include <TimeLib.h>
   
  class Timer{
    public:
      int t = -1;
      void reset(void);
      void addTime(void);
    private:
      int lastT = 0;
  };

  class CompoundTimer
  {
  public:
    int storedTime = 0;
    int t = -1;
    void reset(void);
    void addTime(void);
    void resumeTimer(void);
    void pauseTimer(void);
    int getTotalTime(void);
  private:
    int lastT = 0;
  };
#endif

