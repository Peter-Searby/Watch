#ifndef HEADER_STATECLASS
  #define HEADER_STATECLASS
  #include "Timers.h"
  //#include "arduino.h"
  #include <Wire.h>
  #include "Buttons.h"
  #include "Sodaq_DS3231.h"
  
  #include <idDHT11.h>
  #include <TimeLib.h>
  #include "graphics.h"
  #include <complex.h>
   
  class State{
    public:
      Button * butts[6];
      bool needsUpdate = false;
      void initia(Button*, Button*, Button*, Button*, Button*, Button*);
      virtual void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306)=0;
      virtual void load(void)=0;
      virtual void unload(void)=0;
      virtual void leave(void)=0;
      virtual void enter(void)=0;
  };
  class ClockState : public State{
    public:
      int lastS;
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
   
  class TimerState : public State{
    public:
      CompoundTimer timerTimer;
      bool timerOn=false;
      bool timerSelected=false;
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
   
  class PictureState : public State{
    public:
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
   
  class GOState : public State{
    public:
      int SelectedCoord[2] = {4, 4};
      bool deletingStonesBoard[9][9];
      bool liberties = false;
      byte getStone(int, int);
      bool boardRepeat();
      void checkForStone(int, int);
      void deleteStones(int, int);
      byte boardStones [9][9];
      byte lastBoardStones [9][9];
      GOState();
      bool WhiteSelected = false;
      void displayBoard(Adafruit_SSD1306);
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
  class Bullet{
    public:
      int x;
      int y;
      bool alive = false;
      bool isSpike;
      void Move(void);
  };
  class Barrier{
    public:
    int x;
    int y;
    byte health;
    unsigned char graphic[16];
    void spawn(int, int);
    void damage(void);
  };
  class Alien{
    public:
      void moveDown(void);
      void moveLeft(void);
      void moveRight(void);
      void spawn(int, int);
      int x;
      int y;
      bool alive = false;
  };
  const int NUMOFALIENS = 30;
  class SIState : public State{
    public:
      int playerX;
      int playerY;
      int omegaX;
      int score;
      Alien aliens[NUMOFALIENS];
      Barrier barriers[3];
      Bullet spikes[40];
      Bullet bullets[10];
      Timer alienMoveTimer;
      Timer alienChangeMoveTimer;
      Timer alienAdvanceTimer;
      Timer alienShootTimer;
      Timer omegaTimer;
      bool movingLeft;
      int subState;
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
      void shoot(void);
  };
  class HumidityState : public State{
    public:
      int humidity;
      void updateHumidity(void);
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
   
  class TempState : public State{
    public:
      int temperature;
      int CPUtemperature;
      void updateTemp(void);
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
  class MandelbrotState : public State{
    public:
      __complex__ float currentZ;
      int currentX;
      int currentY;
      int timeSpent;
      int maxTime;
      bool image[64][128];
      bool getInSet(int, int);
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
  class BlankState : public State{
    public:
      void draw(byte, byte, byte, byte, byte, byte, bool*, Adafruit_SSD1306);
      void load(void);
      void unload(void);
      void leave(void);
      void enter(void);
  };
   
#endif
