
#include "Buttons.h"

Button::Button()
{
}

Button::Button(int i, int t, bool r, int p)
{
  id = i;
  t1 = t;
  repeat = r;
  repeatPeriod = p;
  timer = Timer();
}


//B1: returned when button pressed
//B10: returned when button released before t1
//B11: returned when timer just passed t1
//B100 returned when button released after t1
//B101 returned for a repeated event past t1
byte Button::getEvent(int bState)
{
  if (bState == 1){
    if (timer.t == -1){
      timer.reset();
      return B1;
    } else {
      timer.addTime();
    }
    if (timer.t > t1 && !overT1){
      overT1 = true;
      if (repeat){
        timer.reset();
      }
      return B11;
    }
    if (repeat && overT1 && timer.t>repeatPeriod){
      timer.reset();
      return B101;
    }
    
  } else if (timer.t > 0){
    overT1 = false;
    if (timer.t < t1){
      timer.t = -1;
      return B10;
    } else {
      timer.t = -1;
      return B100;
    }
    timer.t = -1;
  }
  return B0;
}



