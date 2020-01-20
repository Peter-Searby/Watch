#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#include "States.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);



///////////////////////////


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define M_PI 3.14159265358979323846


// Creating buttons
Button * leftButt = new Button(21, 750, 1, 400);
Button * rightButt = new Button(14, 750, 1, 400);
Button * upButt = new Button(22, 750, 1, 400);
Button * downButt = new Button(15, 750, 1, 400);
Button * homeButt = new Button(23, 750, 0, 400);
Button * selectButt = new Button(20, 750, 0, 400);







Timer frameKeeper;

int curState[2] = {0, 0};

bool selectToggle;


volatile int lbState = 0;
volatile int rbState = 0;
volatile int ubState = 0;
volatile int dbState = 0;
volatile int hbState = 0;
volatile int sbState = 0;
#define STATESHEIGHT 4
#define STATESWIDTH 2


void buttonUpdateL(){
  lbState = !digitalRead(21);
}

void buttonUpdateR(){
  rbState = !digitalRead(14);
}
void buttonUpdateU(){
  ubState = !digitalRead(22);
}
void buttonUpdateD(){
  dbState = !digitalRead(15);
}
void buttonUpdateH(){
  hbState = !digitalRead(23);
}
void buttonUpdateS(){
  sbState = !digitalRead(20);
}





time_t getTeensyTime()
{
  return Teensy3Clock.get();
}

time_t getRTCTime()
{
  return rtc.now().getEpoch();
}








State* states[STATESHEIGHT][STATESWIDTH] {new ClockState, new TimerState, new GOState, new SIState, new HumidityState, new TempState, new BlankState, new MandelbrotState};



void setup() {
  // put your setup code here, to run once
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
//  display.setRotation(2);
  display.clearDisplay();
//  display.drawBitmap(32, 0, img, 64, 64, WHITE);
  display.display();
  int startTime = millis();
  Wire.begin();
  Serial.begin(9600);
  rtc.begin();
  setSyncProvider(getRTCTime);   // the function to get the time from the RTC
  setSyncInterval(0.5f);
  pinMode(23, INPUT);
  digitalWrite(23, HIGH);
  attachInterrupt(digitalPinToInterrupt(23), buttonUpdateH, CHANGE);
  pinMode(22, INPUT);
  digitalWrite(22, HIGH);
  attachInterrupt(digitalPinToInterrupt(22), buttonUpdateU, CHANGE);
  pinMode(21, INPUT);
  digitalWrite(21, HIGH);
  attachInterrupt(digitalPinToInterrupt(21), buttonUpdateL, CHANGE);
  pinMode(20, INPUT);
  digitalWrite(20, HIGH);
  attachInterrupt(digitalPinToInterrupt(20), buttonUpdateS, CHANGE);
  pinMode(15, INPUT);
  digitalWrite(15, HIGH);
  attachInterrupt(digitalPinToInterrupt(15), buttonUpdateD, CHANGE);
  pinMode(14, INPUT);
  digitalWrite(14, HIGH);
  attachInterrupt(digitalPinToInterrupt(14), buttonUpdateR, CHANGE);
  frameKeeper.reset();
  for (int i = 0; i<6; i++){
    int a = i/2;
    int b = i%2;
    states[a][b]->initia(leftButt, rightButt, upButt, downButt, selectButt, homeButt);
  }
  states[curState[0]][curState[1]]->load();
//  delay(400-(millis()-startTime));
//  display.setRotation(3);
//  display.clearDisplay();
//  display.drawBitmap(0, 32, img, 64, 64, WHITE);
//  display.display();
//  delay(400);
//  display.setRotation(0);
//  display.clearDisplay();
//  display.drawBitmap(32, 0, img, 64, 64, WHITE);
//  display.display();
//  delay(400);
//  display.setRotation(1);
//  display.clearDisplay();
//  display.drawBitmap(0, 32, img, 64, 64, WHITE);
//  display.display();
//  delay(400);
  display.setRotation(2);
//  display.clearDisplay();
//  display.drawBitmap(32, 0, img, 64, 64, WHITE);
//  display.display();
//  delay(400);
}





byte readByte() {
 while (!Serial.available()) delay(10);
 byte reading = 0;
 byte incomingByte = Serial.read();
 while (incomingByte != '\n') {
   if (incomingByte >= '0' && incomingByte <= '9')
     reading = reading * 10 + (incomingByte - '0');
   else;
   incomingByte = Serial.read();
 }
 Serial.flush();
 return reading;
}






void drawArm(float progress, int armLen) {
  float armRadians = (M_PI * 2 * progress) - (M_PI/2);

  float targetX = 18 + cos(armRadians) * armLen;
  float targetY = 18 + sin(armRadians) * armLen;

  display.drawLine(18, 18, targetX, targetY, WHITE);
}


void loop() {
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      rtc.setEpoch(t);
      setTime(t);
    }
  }


  byte lB = leftButt->getEvent(lbState);
  byte rB = rightButt->getEvent(rbState);
  byte uB = upButt->getEvent(ubState);
  byte dB = downButt->getEvent(dbState);
  byte hB = homeButt->getEvent(hbState);
  byte sB = selectButt->getEvent(sbState);



  if (!(selectToggle&&selectButt->timer.t<selectButt->t1)) {
    if((uB==B1||uB==B101)&&curState[0]>0){
      states[curState[0]][curState[1]]->unload();
      curState[0] -= 1;
      states[curState[0]][curState[1]]->load();
    }
    if((dB==B1||dB==B101)&&curState[0]<STATESHEIGHT-1){
      states[curState[0]][curState[1]]->unload();
      curState[0] += 1;
      states[curState[0]][curState[1]]->load();
    }
    if((lB==B1||lB==B101)&&curState[1]>0){
      states[curState[0]][curState[1]]->unload();
      curState[1] -= 1;
      states[curState[0]][curState[1]]->load();
    }
    if((rB==B1||rB==B101)&&curState[1]<STATESWIDTH-1){
      states[curState[0]][curState[1]]->unload();
      curState[1] += 1;
      states[curState[0]][curState[1]]->load();
    }
    if(hB==B1){
      states[curState[0]][curState[1]]->unload();
      curState[0] = 0;
      curState[1] = 0;
      states[curState[0]][curState[1]]->load();
    }
  }
  states[curState[0]][curState[1]]->draw(lB, rB, uB, dB, hB, sB, &selectToggle, display);
}






#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = DEFAULT_TIME; // return 0 to indicate that the time is not valid
     }
  } else {
    Serial.write("No Time header");
  }
  return pctime;

}
