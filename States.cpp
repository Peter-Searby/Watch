#include "States.h"

int idDHT11pin = 16;
int idDHT11intNumber = 16;

void dht11_wrapper();

idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);


void dht11_wrapper() {
  DHT11.isrCallback();
}

Timer frameKeeper2;

bool colon;
void State::initia(Button * lB, Button * rB, Button * uB, Button * dB, Button * sB, Button * hB){
  butts[0] = lB;
  butts[1] = rB;
  butts[2] = uB;
  butts[3] = dB;
  butts[4] = sB;
  butts[5] = hB;
}
void ClockState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display) {
    needsUpdate = false;
    int s = second();
    if (*selectToggle){
      if(lB==B10){
        time_t t = now();
        t -= 60;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
      if (lB==B11){
        time_t t = now();
        t -= 1;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
      if (rB==B10){
        time_t t = now();
        t += 60;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
      if (rB==B11){
        time_t t = now();
        t += 1;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
      if(uB==B10||uB==B11){
        time_t t = now();
        t += 3600;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
      if(dB==B10||dB==B11){
        time_t t = now();
        t -= 3600;
        rtc.setEpoch(t);
        setTime(t);
        needsUpdate = true;
      }
    }
    if (s != lastS || needsUpdate){
      colon = !colon;
      display.clearDisplay();
      display.setCursor(1, 16);
      display.setTextSize(4.5);
      display.setTextColor(WHITE);
    
      int h = hour();
      int mi = minute();
      
      int d = day();
      int mo = month();
      int y = year();
      
      
      display.print(h/10);
      display.print(h%10);
      if (colon){
        display.print(":");
      } else {
        display.print(" ");
      }
      display.print(mi/10);
      display.print(mi%10);
      
      display.setCursor(31, 0);
      display.setTextSize(1);
      display.print("(");
      display.print(d);
      display.print("/");
      display.print(mo);
      display.print("/");
      display.print(y);
      display.print(")");
      
      display.setTextSize(2);
      display.setCursor(45, 44);
      display.print(s/10);
      display.print(s%10);
      if (*selectToggle){
        display.drawRect(0, 0, 128, 64, WHITE);
      }
      display.display();
    }

    
    if (*selectToggle){
      if(hB==B1){
        *selectToggle = false;
        display.drawRect(0, 0, 128, 64, BLACK);
        display.display();
      }
    } else {
      if (sB==B1){
        display.drawRect(0, 0, 128, 64, WHITE);
        display.display();
        *selectToggle = true;
      }
    }
    
    lastS = s;
}
void ClockState::load(void){
  needsUpdate = true;
}
void ClockState::unload(void){
  
}
void ClockState::leave(void){
  
}
void ClockState::enter(void){
  
}

void PictureState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  if (needsUpdate){
    needsUpdate = false;
    display.clearDisplay();
    display.drawBitmap(32, 0, img, 64, 64, WHITE);
    display.display();
  }
}

void PictureState::load(void){
  needsUpdate = true;
}
void PictureState::unload(void){
  
}
void PictureState::leave(void){
  
}
void PictureState::enter(void){
  
}

void GOState::checkForStone(int x, int y){
  byte colour = getStone(x, y);
  int xs[] = {0, 1, 0, -1};
  int ys[] = {-1, 0, 1, 0}; 
  int x2;
  int y2;
  for (int i=0;i<4;i++){
    x2 = x+xs[i];
    y2 = y+ys[i];
    if (getStone(x2, y2) == colour){
      if(!deletingStonesBoard[y2][x2]){
        deletingStonesBoard[y2][x2] = true;
        checkForStone(x2, y2);
      }
    } else if (getStone(x2, y2) == B0){
      liberties = true;
    }
  }
}

void GOState::deleteStones(int x, int y){
  byte colour = getStone(x, y);
  byte eColour = colour^B11;
  int xs[] = {0, 1, 0, -1};
  int ys[] = {-1, 0, 1, 0}; 
  int x2;
  int y2;
  for (int i=0; i<4; i++){
    x2 = x+xs[i];
    y2 = y+ys[i];
    if (getStone(x2, y2) == eColour){
      liberties = false;
      deletingStonesBoard[y2][x2] = true;
      checkForStone(x2, y2);
      if (!liberties){
        for (int j=0; j<9; j++){
          for (int k=0; k<9; k++){
            if(deletingStonesBoard[j][k]){
              boardStones[j][k] = B0;
              deletingStonesBoard[j][k] = false;
            }
          }
        }
      } else {
        for (int j=0; j<9; j++){
          for (int k=0; k<9; k++){
            deletingStonesBoard[j][k] = false;
          }
        }
      }
    }
  }
}

byte GOState::getStone(int x, int y){
  if (x<0||x>8||y<0||y>8){
    return B11;
  }
  return boardStones[y][x];
}

GOState::GOState(){
  for (int i = 0;i<9;i++){
    for (int j = 0; j<9; j++){
      boardStones[i][j] = B0;
    }
  }
}

void GOState::displayBoard(Adafruit_SSD1306 display){
  for (int i = 0;i<9;i++){
    for (int j = 0;j<9;j++){
      if (boardStones[j][i] == 0){
        display.drawBitmap(i*5+37, j*5+10, GOBackGraphics[boardBackground[j][i]], 5, 5, WHITE);
      } else {
        display.drawBitmap(i*5+37, j*5+10, GOStonesGraphics[boardStones[j][i]-1], 5, 5, WHITE);
      }
    }
  }
}

bool GOState::boardRepeat() {
  for (int i = 0;i<9;i++){
    for (int j = 0;j<9;j++){
      if (boardStones[j][i] != lastBoardStones[j][i]) {
        return false;
      }
    }
  }
  return true;
}
void GOState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){    
  needsUpdate = true;
  display.clearDisplay();
  if (*selectToggle){
    display.drawRect(0, 0, 128, 64, WHITE);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    if(WhiteSelected) {
      display.print("White's turn");
    } else {
      display.print("Black's turn");
    }
    if((uB==B1||uB==B11)&&SelectedCoord[1]>0){
      SelectedCoord[1] -= 1;
    }
    if((dB==B1||dB==B11)&&SelectedCoord[1]<=7){
      SelectedCoord[1] += 1;
    }
    if((lB==B1||lB==B11)&&SelectedCoord[0]>0){
      SelectedCoord[0] -= 1;
    }
    if((rB==B1||rB==B11)&&SelectedCoord[0]<=7){
      SelectedCoord[0] += 1;
    }
    if(hB==B1){
      *selectToggle = false;
    }
    if (sB==B1){
      if (boardStones[SelectedCoord[1]][SelectedCoord[0]]==B0){
        byte tempBoard[9][9];
        int s = 9*9*sizeof(byte);
        memcpy(tempBoard, boardStones, s);
        if (WhiteSelected){
          boardStones[SelectedCoord[1]][SelectedCoord[0]] = B1;
        } else {
          boardStones[SelectedCoord[1]][SelectedCoord[0]] = B10;
        }
        deleteStones(SelectedCoord[0], SelectedCoord[1]);
        if (boardRepeat()) {
          memcpy(boardStones, tempBoard, s);
        } else {
          WhiteSelected = !WhiteSelected;
          memcpy(lastBoardStones, tempBoard, s);
        }
      }
    }
  } else {
    if (sB==B1){
      *selectToggle = true;
    }
  }
  displayBoard(display);
  if (*selectToggle){
    int x = SelectedCoord[0]*5+37;
    int y = SelectedCoord[1]*5+10;
    if (boardStones[SelectedCoord[1]][SelectedCoord[0]] == B10){
      display.drawBitmap(x, y, GOSelected, 5, 5, WHITE);
    } else {
      display.drawBitmap(x, y, GOSelected, 5, 5, BLACK);
    }
  }
  display.display();
}

void GOState::load(void){
  
}
void GOState::unload(void){
  
}
void GOState::leave(void){
  
}
void GOState::enter(void){
  
}
void Barrier::spawn(int x, int y){
  this->x = x;
  this->y = y;
  health = 8;
  for (int i=0;i<16;i++){
    graphic[i] = BarrierGraphic[i];
  }
}
void Barrier::damage(void){
  health-=1;
  switch (health){
    case 0:
      break;
    case 1:
      graphic[2] = 0x00;
      graphic[3] = 0x00;
      break;
    case 2:
      graphic[2] = 0x40;
      graphic[3] = 0x00;
      break;
    case 3:
      graphic[2] = 0x50;
      graphic[3] = 0x00;
      break;
    case 4:
      graphic[2] = 0x54;
      graphic[3] = 0x00;
      break;
    case 5:    
      graphic[2] = 0x55;
      graphic[3] = 0x00;
      break;
    case 6:
      graphic[2] = 0x55;
      graphic[3] = 0x40;
      break;
    case 7:
      graphic[2] = 0x55;
      graphic[3] = 0x50;
      break;
    case 8:
      graphic[2] = 0x55;
      graphic[3] = 0x54;
      break;
    default:
      break;
      
  }
}
void Alien::moveDown(void){
  if (y<52){
    y += 4;
  }
}
void Alien::moveLeft(void){
  x -= 2;
}
void Alien::moveRight(void){
  x += 2;
}
void Alien::spawn(int x, int y){
  alive = true;
  this->x = x;
  this->y = y;
}

void Bullet::Move(void){
  if(isSpike){
    y+=4;
  } else {
    y-=4;
  }
  if (y<0||y>64){
    alive = false;
  }
}

bool areColliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
  return x1+w1>x2&&x1<x2+w2&&y1+h1>y2&&y1<y2+h2;
}

void SIState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  display.clearDisplay();
  display.setTextColor(WHITE);
  int m;
  int n;
  switch(subState){
    case 0:
    
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(28, 0);
      display.print("COSMOS");
      display.setCursor(10, 18);
      display.print("INTRUDERS");
      for (int i = 0; i<5; i++){
        display.drawBitmap(17+i*20, 36, IntruderGraphic, 8, 8, WHITE);
        display.drawBitmap(14+i*20, 45, BarrierGraphic, 14, 8, WHITE);
        display.drawBitmap(17+i*20, 54, FalconGraphic, 8, 8, WHITE);
      }
      if (sB==B1){
        *selectToggle = true;
        enter();
      }
      break;
      
    case 1:


      //OMEGA
      omegaTimer.addTime();
      if (omegaTimer.t>30000){
        omegaX = -16;
        omegaTimer.reset();
      }
      if (omegaX<128){
        omegaX+=2;
        display.drawBitmap(omegaX, 0, OmegaGraphic, 16, 4, WHITE);
      }
      //Alien Advancement
      alienAdvanceTimer.addTime();
      if (alienAdvanceTimer.t>10000){
        for (int i = NUMOFALIENS-1; i>=0; i--){
          bool colliding = false;
          for (int j=0; j<NUMOFALIENS; j++){
            if (i!=j){
              if (areColliding(aliens[j].x, aliens[j].y, 8, 8, aliens[i].x, aliens[i].y+5, 8, 8)){
                colliding = true;
              }
            }
          }
          if (!colliding){
            aliens[i].moveDown();
            if (aliens[i].y>48){
              subState = 2;
            }
            for (int j=0;j<3;j++){
              if (barriers[j].health>0){
                if (areColliding(barriers[j].x, barriers[j].y, 14, 8, aliens[i].x, aliens[i].y, 8, 8)){
                  aliens[i].alive = false;
                  barriers[j].damage();
                }
              }
            }
          }
        }
        alienAdvanceTimer.reset();
      }


      //Alien changing direction
      alienChangeMoveTimer.addTime();
      if (alienChangeMoveTimer.t>random(5, 10)*500){
        alienChangeMoveTimer.reset();
        movingLeft = random(2)==1;
      }


      //Alien movement
      alienMoveTimer.addTime();
      m = 1;
      n = 1;
      for (int i = 0; i<NUMOFALIENS; i++){
        if (aliens[i].x>aliens[m].x&&aliens[i].alive){
          m = i;
        }
        if (aliens[i].x<aliens[n].x&&aliens[i].alive){
          n = i;
        }
      }
      if (aliens[m].x>120){
        movingLeft = true;
      }
      if (aliens[n].x<0){
        movingLeft = false;
      }
      if (alienMoveTimer.t>400){
        if (movingLeft){
          for (int i = 0; i<NUMOFALIENS; i++){
            aliens[i].moveLeft();
          }
        } else {
          for (int i = 0; i<NUMOFALIENS; i++){
            aliens[i].moveRight();
          }
        }
        alienMoveTimer.reset();
      }


      //Alien Shooting
      alienShootTimer.addTime();
      if (alienShootTimer.t>750){
        Alien shooters[NUMOFALIENS];
        int total = 0;
        for (int i = 0;i<NUMOFALIENS;i++){
          if (aliens[i].alive){
            bool colliding = false;
            for (int j=0;j<NUMOFALIENS;j++){
              if (j!=i&&aliens[j].alive){
                if (aliens[i].y<aliens[j].y&&aliens[i].x<aliens[j].x+8&&aliens[i].x+8>aliens[j].x){
                  colliding = true;
                }
              }
            }
            if (!colliding){
              shooters[total].x = aliens[i].x;
              shooters[total].y = aliens[i].y;
              total++;
            }
          }
        }
        if (total>0){
          int j = random(0, total);
          
          for(int i=0; i<NUMOFALIENS; i++){
            if (!spikes[i].alive){
              spikes[i].alive = true;
              spikes[i].x = shooters[j].x+2;
              spikes[i].y = shooters[j].y+4;
              break;
            }
          }
          alienShootTimer.reset();
        }
      }
      display.drawBitmap(playerX, playerY, FalconGraphic, 8, 8, WHITE);
      for (int i = 0; i<NUMOFALIENS; i++){
        if (aliens[i].alive){
          display.drawBitmap(aliens[i].x, aliens[i].y, IntruderGraphic, 8, 8, WHITE);
        }
      }
      for (int i = 0; i<NUMOFALIENS; i++){
        if (spikes[i].alive){
          display.drawBitmap(spikes[i].x, spikes[i].y, SpikeGraphic, 3, 3, WHITE);
        }
      }
      for (int i = 0; i<10; i++){
        if (bullets[i].alive){
          display.drawBitmap(bullets[i].x, bullets[i].y, BulletGraphic, 4, 4, WHITE);
        }
      }

      for (int i=0;i<3;i++){
        if(barriers[i].health>0){
          display.drawBitmap(barriers[i].x, barriers[i].y, barriers[i].graphic, 14, 8, WHITE);
          for (int j=0; j<NUMOFALIENS;j++){
            if (spikes[j].alive){
              if (areColliding(spikes[j].x, spikes[j].y, 3, 3, barriers[i].x, barriers[i].y, 14, 8)){
                barriers[i].damage();
                spikes[j].alive = false;
              }
            }
          }
          for (int j=0; j<10;j++){
            if (bullets[j].alive){
              if (areColliding(bullets[j].x, bullets[j].y, 4, 4, barriers[i].x, barriers[i].y, 14, 8)){
                barriers[i].damage();
                bullets[j].alive = false;
              }
            }
          }
        }
      }
      
      if (hB==B1){
        subState = 2;
      }

      //Player moving
      if ((lB==B101||lB==B1)&&playerX>2){
        playerX-=3;
      }
      if ((rB==B101||rB==B1)&&playerX<118){
        playerX+=3;
      }
      // Bullets moving
      for (int i=0;i<NUMOFALIENS;i++){
        if (spikes[i].alive){
          spikes[i].Move();
          if (areColliding(playerX, playerY, 8, 8, spikes[i].x, spikes[i].y, 3, 3)){
            subState = 2;
            break;
          }
        }
      }
      for (int i=0;i<10;i++){
        if (bullets[i].alive){
          bullets[i].Move();
          for (int j=0;j<NUMOFALIENS;j++){
            if (aliens[j].alive){
              if (areColliding(aliens[j].x, aliens[j].y, 8, 8, bullets[i].x, bullets[i].y, 4, 4)){
                aliens[j].alive = false;
                score++;
                bullets[i].alive = false;
                break;
              }
            }
          }
          if (omegaX<128){
            if(areColliding(omegaX, 0, 16, 4, bullets[i].x, bullets[i].y, 4, 4)){
              score += 10;
              omegaX = 128;
            }
          }
        }
      }

      //Player Shooting
      if (dB==B101||dB==B1){
        for(int i=0; i<10; i++){
          if (!bullets[i].alive){
            bullets[i].alive = true;
            bullets[i].x = playerX+2;
            bullets[i].y = playerY+4;
            break;
          }
        }
      }

      
      break;
    case 2:
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(16, 16);
      display.print("You Died");
      display.setCursor(0, 34);
      display.setTextSize(1);
      display.print("Your score was: ");
      display.print(score);
      if (hB==B1){
        subState = 0;
        *selectToggle = false;
      }
      break;
    default:
      break;
  } 
  display.display();
}

void SIState::load(void){
  subState = 0;
  needsUpdate = true;
}
void SIState::unload(void){
  
}
void SIState::leave(void){
  subState = 0;
}
void SIState::enter(void){
  subState = 1;
  score = 0;
  omegaX = 128;
  for (int i=0; i<NUMOFALIENS;i++){
    if (i<10){
      aliens[i].spawn(i*12, 8);
    } else if (i<20){
      aliens[i].spawn((i-10)*12, 20);
    } else{
      aliens[i].spawn((i-20)*12, 32);
    }
  }
  for (int i=0;i<3;i++){
    barriers[i].spawn(22+35*i, 48);
  }
  butts[0]->t1 = 0;
  butts[1]->t1 = 0;
  butts[0]->repeatPeriod = 50;
  butts[1]->repeatPeriod = 50;
  butts[3]->t1 = 0;
  butts[3]->repeatPeriod = 750;
  for (int i=0;i<NUMOFALIENS;i++){
    spikes[i].alive = false;
    spikes[i].isSpike = true;
  }
  for (int i=0;i<10;i++){
    bullets[i].alive = false;
    bullets[i].isSpike = false;
  }
  playerX = 60;
  playerY = 56;
  omegaTimer.reset();
  alienMoveTimer.reset();
  alienChangeMoveTimer.reset();
  alienAdvanceTimer.reset();
  alienShootTimer.reset();
}
void HumidityState::updateHumidity(void){
  humidity = DHT11.getHumidity();
  DHT11.acquire();
}
void HumidityState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
    frameKeeper2.addTime();
    if (!DHT11.acquiring() && frameKeeper2.t > 1000){
      frameKeeper2.reset();
      updateHumidity();
      display.clearDisplay();
      display.setCursor(30, 16);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Humidity: ");
      if (humidity != -7){
        display.print(humidity);
      } else {
        display.print("~");
      }
      display.display();
    }
 }
void HumidityState::load(void){
  needsUpdate = true;
}
void HumidityState::unload(void){
  
}
void HumidityState::leave(void){
  
}
void HumidityState::enter(void){
  
}
void TempState::updateTemp(void){
  temperature = DHT11.getCelsius();
  DHT11.acquire();
  rtc.convertTemperature();
  CPUtemperature = rtc.getTemperature();
}

void TempState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  frameKeeper2.addTime();
  if (!DHT11.acquiring() && frameKeeper2.t > 1000){
    frameKeeper2.reset();
    updateTemp();
    display.clearDisplay();
    display.setCursor(28, 20);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Temp|CPU Temp");
    display.setCursor(28, 30);
    display.print("-------------");
    display.setCursor(40, 40);
    if (temperature != -7){
      display.print(temperature);
    } else{
      display.print("~");
    }
    display.print("|");
    if (CPUtemperature != -7){
      display.print(CPUtemperature);
    } else {
      display.print("~");
    }
    display.display();
  }
}

void TempState::load(void){
  needsUpdate = true;
}
void TempState::unload(void){
  
}
void TempState::leave(void){
  
}
void TempState::enter(void){
  
}
void TimerState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  if (dB==B1&&*selectToggle){
    timerSelected = true;
  } else if (uB==B1&&*selectToggle){
    timerSelected = false;
  }
  if (timerOn){
    display.setCursor(36, 2);
    display.print("Stop");
    timerTimer.addTime();
  } else {
    display.setCursor(28, 2);
    display.print("Start");
  }
  display.setCursor(28, 20);
  display.print("Reset");
  if (timerSelected){
    display.drawRect(26, 18, 64, 18, WHITE);
    if (sB == B1&&*selectToggle){
      timerTimer.reset();
    }
  } else {
    display.drawRect(26, 0, 64, 18, WHITE);
    if (timerOn&&sB==B1&&*selectToggle){
      timerOn = false;
      timerTimer.pauseTimer();
    } else if (!timerOn && sB==B1&&*selectToggle){
      timerOn = true;
      timerTimer.resumeTimer();
      
    }
  }
  int h = hour(timerTimer.getTotalTime()/1000);
  int m = minute(timerTimer.getTotalTime()/1000);
  int s = second(timerTimer.getTotalTime()/1000);
  int ms = timerTimer.getTotalTime()%1000;
  display.setCursor(10, 40);
  display.setTextColor(WHITE);
  if (h>0){
    display.print(h/10);
    display.print(h%10);
    display.print(":");
  }
  display.print(m/10);
  display.print(m%10);
  display.print(":");
  display.print(s/10);
  display.print(s%10);
  if (h==0){
    display.print(":");
    display.print(ms/100);
    display.print(ms%100);
  }if (*selectToggle){
    display.drawRect(0, 0, 128, 64, WHITE);
    if (hB==B1){
      *selectToggle = false;
      timerSelected = false;
    }
  } else if (sB==B1){
    *selectToggle = true;
  }
  display.display();
}

void TimerState::load(void){
  
  needsUpdate = true;
}
void TimerState::unload(void){
  
}
void TimerState::leave(void){
  
}
void TimerState::enter(void){
  
}


void BatteryState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  display.clearDisplay();
  display.setCursor(28, 20);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Battery: ");
  display.setCursor(28, 30);
  display.print("-------------");
  display.setCursor(40, 40);
  display.display();
//    display.print();
}

void BatteryState::load(void){
  needsUpdate = true;
}
void BatteryState::unload(void){
  
}
void BatteryState::leave(void){
  
}
void BatteryState::enter(void){
  
}


void MandelbrotState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  display.clearDisplay();
  display.drawBitmap(0, 0, mandelbrot, 128, 64, WHITE);
  display.display();
}

void MandelbrotState::load(void){
  needsUpdate = true;
}
void MandelbrotState::unload(void){
  
}
void MandelbrotState::leave(void){
  
}
void MandelbrotState::enter(void){
  
}

void BlankState::draw(byte lB, byte rB, byte uB, byte dB, byte hB, byte sB, bool *selectToggle, Adafruit_SSD1306 display){
  display.clearDisplay();
  display.display();
}

void BlankState::load(void){
  needsUpdate = true;
}
void BlankState::unload(void){
  
}
void BlankState::leave(void){
  
}
void BlankState::enter(void){
  
}

