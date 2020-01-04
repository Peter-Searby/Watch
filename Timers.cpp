#include "Timers.h"

void Timer::reset(void){
  t = 0;
  lastT = millis();
}
void Timer::addTime(void){
  t += millis()-lastT;
  lastT = millis();
}


void CompoundTimer::reset(void){
  t = 0;
  lastT = millis();
  storedTime = 0;
}
void CompoundTimer::addTime(void){
  t += millis()-lastT;
  lastT = millis();
}
void CompoundTimer::pauseTimer(void){
  storedTime += t;
  t = 0;
}
void CompoundTimer::resumeTimer(void){
  t = 0;
  lastT = millis();
}
int CompoundTimer::getTotalTime(void){
  if (t==-1){
    return 0;
  } else {
    return storedTime + t;
  }
}

