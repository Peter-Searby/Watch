
//#include "arduino.h"
#include "Timers.h"
class Button
{
  public:
    int id;
    int t1;
    bool repeat;
    int repeatPeriod;
    bool overT1;
    byte getEvent(int bState);
    Button();
    Button(int i, int m, bool r, int p);
    Timer timer;
};

