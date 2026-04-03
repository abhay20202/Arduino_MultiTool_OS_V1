
//Author = Abhay pawar

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BTN_UP = 2;
const int BTN_DOWN = 3;
const int BTN_LEFT = 4;
const int BTN_RIGHT = 5;
const int BTN_SEL = 6; 
const int BTN_BACK = 7; 
const int PIN_EMF = A0; 

enum State { MENU, RADAR, DISTANCE, SAFE, REFLEX, LIE, GHOST, TIMER, SONAR, INFO };
State currentState = MENU;

const char* menuItems[] = {
  "1. 360 Scanner",      
  "2. Laser Tape",       
  "3. Safe Cracker", 
  "4. Reflex Master", 
  "5. Lie Detector",
  "6. EMF Scanner",    
  "7. Stopwatch",       
  "8. Sonar Bird",
  "9. System Info"
};
int menuIndex = 0;
const int totalApps = 9;

byte sharedGraph[128]; 

bool scannerPaused = false;

int gameX = 128, gameGap = 20, playerY = 32, gameScore = 0;
bool gameOver = false;

int safeLevel = 1; int safeProgress = 0; int safeTarget = 0; bool safeLocked = true;

unsigned long reflexStart = 0; unsigned long reflexReaction = 0;
bool reflexWaiting = true; bool reflexShowResult = false;
unsigned long reflexRandomDelay = 0; int reflexCheatCount = 0;

int lieTimer = 0; long lieShakeScore = 0;
bool lieAnalyzing = false; bool lieResultShown = false; bool lieIsTruth = false;

unsigned long stopStart = 0;
unsigned long stopElapsed = 0;
bool stopRunning = false;

// --- ANIMATED SECRET BITMAPS ---
const unsigned char smile_frame1[] PROGMEM = {
  0x03, 0xC0, 0x0F, 0xF0, 0x1C, 0x38, 0x30, 0x0C, 
  0x20, 0x04, 0x66, 0x66, 0x46, 0x62, 0x40, 0x02, 
  0x40, 0x02, 0x41, 0x82, 0x60, 0x06, 0x20, 0x04, 
  0x30, 0x0C, 0x18, 0x18, 0x0F, 0xF0, 0x03, 0xC0
};

const unsigned char smile_frame2[] PROGMEM = {
  0x03, 0xC0, 0x0F, 0xF0, 0x1C, 0x38, 0x30, 0x0C, 
  0x20, 0x04, 0x66, 0x06, 0x46, 0x02, 0x40, 0x02, 
  0x40, 0x02, 0x41, 0x82, 0x60, 0x06, 0x20, 0x04, 
  0x30, 0x0C, 0x18, 0x18, 0x0F, 0xF0, 0x03, 0xC0
};

long getSmartDistance(bool precisionMode); 
void runMenu();
void runScanner();
void runDistance();
void runSafeCracker();
void runReflex();
void runLieDetector();
void runGhost();
void runStopwatch();
void runSonarBird();
void runInfo();

void setup() {
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  pinMode(BTN_UP, INPUT_PULLUP); pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP); pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SEL, INPUT_PULLUP); pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(PIN_EMF, INPUT); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for(;;); }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2); display.setCursor(35, 15); display.println(F("ABHAY"));
  display.setTextSize(1); display.setCursor(38, 35); display.println(F("CREATIONS"));
  display.display();
  delay(500);

  for(int i = 20; i <= 108; i += 4) {
    display.drawRect(20, 50, 88, 6, WHITE);
    display.fillRect(20, 50, i - 20, 6, WHITE);
    display.display();
    delay(30);
  }
}

void loop() {
  
  if (digitalRead(BTN_BACK) == LOW && digitalRead(BTN_UP) == LOW) {
    int frameCount = 0;
    while(digitalRead(BTN_BACK) == LOW || digitalRead(BTN_UP) == LOW) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10, 10);
      display.println(F("Thanks For Testing"));
      display.setCursor(20, 25);
      display.println(F("My Project : )"));
      
      if ((frameCount / 2) % 2 == 0) {
        display.drawBitmap(56, 40, smile_frame1, 16, 16, WHITE);
      } else {
        display.drawBitmap(56, 40, smile_frame2, 16, 16, WHITE);
      }
      display.display();
      delay(150); 
      frameCount++;
    }
    currentState = MENU; 
    delay(200); 
    return;
  } 
  else if (digitalRead(BTN_BACK) == LOW) { 
    currentState = MENU; delay(200); 
  }
  
  display.clearDisplay();
  
  switch (currentState) {
    case MENU:      runMenu(); break;
    case RADAR:     runScanner(); break; 
    case DISTANCE:  runDistance(); break;
    case SAFE:      runSafeCracker(); break;
    case REFLEX:    runReflex(); break;
    case LIE:       runLieDetector(); break;
    case GHOST:     runGhost(); break;
    case TIMER:     runStopwatch(); break;
    case SONAR:     runSonarBird(); break;
    case INFO:      runInfo(); break;
  }
  
  display.display();
}

long getSmartDistance(bool precisionMode) {
  int samples = precisionMode ? 5 : 3; 
  long readings[5]; 
  
  for (int i = 0; i < samples; i++) {
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 24000); 
    if (duration == 0) duration = 24000; 
    readings[i] = duration * 0.034 / 2;
    delay(2);
  }

  if (precisionMode) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4 - i; j++) {
        if (readings[j] > readings[j + 1]) {
          long temp = readings[j];
          readings[j] = readings[j + 1];
          readings[j + 1] = temp;
        }
      }
    }
    return readings[2]; 
  } else {
    long total = 0;
    for(int i=0; i<samples; i++) total += readings[i];
    return total / samples;
  }
}

void runMenu() {
  if (digitalRead(BTN_DOWN) == LOW) { menuIndex++; if (menuIndex >= totalApps) menuIndex = 0; delay(150); }
  if (digitalRead(BTN_UP) == LOW) { menuIndex--; if (menuIndex < 0) menuIndex = totalApps - 1; delay(150); }
  
  if (digitalRead(BTN_SEL) == LOW) {
    if (menuIndex == 0) { scannerPaused = false; currentState = RADAR; } 
    if (menuIndex == 1) currentState = DISTANCE;
    if (menuIndex == 2) { safeLevel=1; safeProgress=0; safeLocked=true; currentState = SAFE; }
    if (menuIndex == 3) { reflexWaiting=true; reflexShowResult=false; reflexCheatCount=0; reflexRandomDelay = millis() + random(2000, 5000); currentState = REFLEX; }
    if (menuIndex == 4) { lieAnalyzing=false; lieResultShown=false; lieTimer=0; lieShakeScore=0; currentState = LIE; }
    if (menuIndex == 5) currentState = GHOST;
    if (menuIndex == 6) { stopRunning = false; stopElapsed = 0; currentState = TIMER; }
    if (menuIndex == 7) { gameScore=0; gameX=128; playerY=32; gameOver=false; currentState = SONAR; }
    if (menuIndex == 8) currentState = INFO;
    delay(200);
  }

  display.setTextSize(1); display.setCursor(15, 0); display.println(F("-- MAIN MENU --"));
  int start = 0; 
  if (menuIndex > 3) start = menuIndex - 3;
  
  for (int i = 0; i < 4; i++) { 
    int itemIndex = start + i;
    if (itemIndex >= totalApps) break;
    display.setCursor(0, 16 + (i * 12));
    if (itemIndex == menuIndex) display.print(F("> ")); else display.print(F("  "));
    display.println(menuItems[itemIndex]);
  }
}

void runGhost() {
  int rawEmf = analogRead(PIN_EMF);
  int emf = map(rawEmf, 0, 800, 0, 63); 
  if (emf > 63) emf = 63;
  if (emf < 2) emf = 0; 

  for (int i = 0; i < 127; i++) {
    sharedGraph[i] = sharedGraph[i+1];
  }
  sharedGraph[127] = 63 - emf;

  display.setTextSize(1); display.setCursor(0,0); display.print(F("EMF SCANNER"));
  display.setCursor(100,0); display.print(emf);
  
  for (int i = 0; i < 128; i++) {
    display.drawFastVLine(i, sharedGraph[i], 64 - sharedGraph[i], WHITE);
  }

  if (emf > 55) {
    display.fillRect(20, 25, 88, 15, BLACK);
    display.drawRect(20, 25, 88, 15, WHITE);
    display.setCursor(30, 29); display.print(F("DETECTED!"));
  }
}

void runStopwatch() {
  if (digitalRead(BTN_SEL) == LOW) {
    if (stopRunning) {
      stopElapsed += millis() - stopStart;
      stopRunning = false;
    } else {
      stopStart = millis();
      stopRunning = true;
    }
    delay(300);
  }
  
  unsigned long current = stopElapsed;
  if (stopRunning) {
    current += millis() - stopStart;
  }
  
  int mins = (current / 1000) / 60;
  int secs = (current / 1000) % 60;
  int ms = (current % 1000) / 10;
  
  display.setTextSize(1); display.setCursor(30, 10); display.print(F("STOPWATCH"));
  display.setTextSize(2); display.setCursor(25, 30);
  
  if (mins < 10) display.print(F("0")); display.print(mins); display.print(F(":"));
  if (secs < 10) display.print(F("0")); display.print(secs);
  
  display.setTextSize(1); display.setCursor(85, 37); 
  display.print(F(".")); if (ms < 10) display.print(F("0")); display.print(ms);
}

void runScanner() {
  if (digitalRead(BTN_SEL) == LOW) { scannerPaused = !scannerPaused; delay(300); }

  if (!scannerPaused) {
    long dist = getSmartDistance(false);
    if (dist > 50) dist = 50; 
    if (dist < 0) dist = 0;
    for (int i = 0; i < 127; i++) sharedGraph[i] = sharedGraph[i+1];
    sharedGraph[127] = map(dist, 0, 50, 63, 0);
  }
  
  for (int x = 0; x < 128; x+=4) { display.drawPixel(x, 32, WHITE); }

  display.setTextSize(1); display.setCursor(0,0);
  if (scannerPaused) {
    display.fillRect(0, 0, 45, 10, WHITE);
    display.setTextColor(BLACK); display.setCursor(2, 1); display.print(F("FROZEN")); display.setTextColor(WHITE);
  } else {
    display.print(F("SCANNING..."));
  }
  for (int i = 0; i < 128; i++) display.drawFastVLine(i, sharedGraph[i], 64 - sharedGraph[i], WHITE);
  if (!scannerPaused) display.drawFastVLine(127, 0, 64, INVERSE);
}

void runDistance() {
  long dist = getSmartDistance(true);
  display.setTextSize(1); display.setCursor(0,0); display.println(F("LASER TAPE"));
  display.setTextSize(3); display.setCursor(20, 25); 
  if (dist > 150) display.print(F("150+")); else display.print(dist); 
  display.setTextSize(1); display.setCursor(100, 40); display.print(F("cm"));
  int bar = map(constrain(dist,0,150), 0, 150, 0, 128); display.fillRect(0, 55, bar, 6, WHITE);
  display.drawRect(0, 53, 128, 10, WHITE);
}

void runLieDetector() {
  if (lieResultShown) {
    display.setTextSize(2); display.setCursor(10, 10);
    if (lieIsTruth) { display.print(F("TRUTH")); display.setTextSize(1); display.setCursor(10, 40); display.print(F("Pulse Stable")); } 
    else { display.print(F("LIE !!!")); display.setTextSize(1); display.setCursor(10, 40); display.print(F("Pulse Unstable")); }
    display.setCursor(10, 55); display.print(F("Press A to Reset"));
    if (digitalRead(BTN_SEL) == LOW) { lieAnalyzing=false; lieResultShown=false; delay(300); } return;
  }
  long dist = getSmartDistance(true); 
  if (!lieAnalyzing) {
    display.setTextSize(1); display.setCursor(10, 0); display.println(F("POLYGRAPH MODE"));
    display.setCursor(10, 20); display.println(F("Place hand at")); display.setCursor(10, 30); display.println(F("15 cm..."));
    display.drawRect(0, 45, 128, 19, WHITE); display.setCursor(50, 50); display.print(dist); display.print("cm");
    if (dist > 10 && dist < 20) {
      display.setCursor(80, 0); display.print(F("[READY]"));
      if (digitalRead(BTN_SEL) == LOW) { lieAnalyzing = true; lieTimer = 0; lieShakeScore = 0; for(int i=0; i<128; i++) sharedGraph[i] = 10; delay(300); }
    }
  } else {
    lieTimer++; 
    int diff = abs(dist - 15); 
    
    if (diff <= 1) diff = 0; 
    
    lieShakeScore += diff; 
    for(int i=0; i<127; i++) sharedGraph[i] = sharedGraph[i+1]; 
    sharedGraph[127] = map(constrain(diff, 0, 10), 0, 10, 18, 0); 
    display.setTextSize(1); display.setCursor(0,0); display.print(F("ANALYZING..."));
    display.drawRect(0, 20, 128, 44, WHITE);
    for(int i=0; i<128; i++) display.drawPixel(i, 20 + sharedGraph[i], WHITE);
  .
    if (lieTimer > 100) { lieResultShown = true; if (lieShakeScore > 50) lieIsTruth = false; else lieIsTruth = true; } 
  }
}

void runSonarBird() {
  if (gameOver) { display.setTextSize(2); display.setCursor(10, 20); display.println(F("CRASHED")); display.setTextSize(1); display.setCursor(30, 45); display.print(F("Score: ")); display.print(gameScore); if (digitalRead(BTN_SEL) == LOW) { gameScore=0; gameX=128; gameOver=false; delay(200); } return; }
  long dist = getSmartDistance(false);
  int targetY = map(constrain(dist, 4, 30), 4, 30, 60, 0);
  playerY = (playerY + targetY) / 2;
  gameX -= 4; if (gameX < -10) { gameX = 128; gameGap = random(10, 35); gameScore++; }
  
  display.fillRect(20, playerY, 8, 5, WHITE); 
  display.drawPixel(26, playerY + 1, BLACK); 
  display.drawLine(20, playerY + 2, 23, playerY + 2, BLACK); 

  display.fillRect(gameX, 0, 8, gameGap, WHITE); 
  display.fillRect(gameX, gameGap + 30, 8, 64, WHITE); 
  display.setCursor(0,0); display.print(gameScore);
  if (gameX < 28 && gameX > 12) { if (playerY < gameGap || playerY + 4 > gameGap + 30) gameOver = true; }
}

void runReflex() {
  if (reflexShowResult) {
    display.setTextSize(1); display.setCursor(10, 10); display.println(F("REACTION TIME:"));
    display.setTextSize(2); display.setCursor(30, 35); display.print(reflexReaction); display.setTextSize(1); display.print(F(" ms"));
    display.setCursor(10, 55); display.print(F("Press A to Retry"));
    if (digitalRead(BTN_SEL) == LOW) { reflexWaiting = true; reflexShowResult = false; reflexCheatCount = 0; reflexRandomDelay = millis() + random(2000, 5000); display.clearDisplay(); display.setCursor(30,30); display.print("RESETTING..."); display.display(); delay(1000); } return;
  }
  if (reflexWaiting) {
    display.setTextSize(1); display.setCursor(40, 30); display.println(F("WAIT..."));
    if (digitalRead(BTN_SEL) == LOW) {
      reflexCheatCount++;
      if (reflexCheatCount > 6) { 
        display.clearDisplay(); display.setTextSize(1); display.setCursor(35, 30); display.println(F("F*CK YOU!!!")); display.display(); delay(3000); reflexCheatCount = 0; reflexRandomDelay = millis() + random(2000, 5000); return; 
      }
      display.clearDisplay(); display.setCursor(20, 30); display.println(F("TOO EARLY!")); display.display(); delay(1000); reflexRandomDelay = millis() + random(2000, 5000);
    }
    if (millis() > reflexRandomDelay) { reflexWaiting = false; reflexStart = millis(); }
  } else {
    display.fillScreen(WHITE); display.setTextColor(BLACK); display.setTextSize(2); display.setCursor(45, 25); display.println(F("GO!")); display.setTextColor(WHITE);
    if (digitalRead(BTN_SEL) == LOW) { reflexReaction = millis() - reflexStart; reflexShowResult = true; reflexCheatCount = 0; delay(1000); }
  }
}

void runSafeCracker() {
  if (safeLevel > 7) { 
    display.setTextSize(2); display.setCursor(20, 20); display.println(F("VAULT")); 
    display.setCursor(25, 40); display.println(F("OPENED!")); 
    if (digitalRead(BTN_SEL) == LOW) { safeLevel = 1; safeProgress = 0; delay(300); }
    return; 
  }
  long dist = getSmartDistance(true);
  const int targets[] = {15, 25, 8, 32, 5, 20, 11};
  safeTarget = targets[safeLevel - 1];

  display.setTextSize(1); display.setCursor(0,0); display.print(F("LEVEL ")); display.print(safeLevel); display.print(F(" TARGET:")); display.print(safeTarget); display.print("cm");
  if (dist >= safeTarget - 1 && dist <= safeTarget + 1) safeProgress += 2; else if (safeProgress > 0) safeProgress -= 5; 
  display.drawRect(10, 30, 108, 12, WHITE); display.fillRect(12, 32, safeProgress, 8, WHITE);
  int cursorX = map(constrain(dist, 0, 45), 0, 45, 0, 128); display.fillTriangle(cursorX, 50, cursorX-3, 55, cursorX+3, 55, WHITE); display.setCursor(50, 55); display.print(dist);
  if (safeProgress >= 104) { display.fillScreen(WHITE); display.display(); delay(100); display.fillScreen(BLACK); display.display(); delay(100); safeLevel++; safeProgress = 0; delay(1000); }
}

void runInfo() {
  display.setTextSize(1); 
  display.setCursor(0,0); display.println(F("ABHAY OS V12"));
  display.println(F("")); 
  display.println(F("Dev: Abhay")); 
  display.println(F("Attempt: 56th"));
  
  display.setCursor(0, 45);
  display.print(F("Uptime: "));
  display.print(millis() / 1000);
  display.print(F(" sec"));
}