#include <LedControl.h>
#include <LiquidCrystal_I2C.h>
#define FORWARD 0
#define LEFT 1
#define RIGHT 2
#define STATE_ON 1
#define STATE_OFF 0

unsigned const int BUTTON_LEFT = 3;
unsigned const int BUTTON_RIGHT = 4;
unsigned const int BUTTON_FORWARD = 5;
unsigned int directionToMove = FORWARD; // By default is set to forward
unsigned int flickeringState = STATE_ON;
unsigned const int DIN = 10;
unsigned const int CS = 9;
unsigned const int CLK = 8;
LedControl lc = LedControl(DIN, CLK, CS, 0);
unsigned int offset = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
const String messages[3] = {"INAINTE", "STANGA", "DREAPTA"};
bool hasChanged = true;
unsigned const int valuePerSlide = 9;
unsigned const int lower = 5;
unsigned const int higher = 1000;
unsigned const int lowestTick = 3000;

byte arrows[3][8][8] = {{{0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x00},
                      {0x3C,0x7E,0x18,0x18,0x18,0x18,0x00,0x18},
                      {0x7E,0x18,0x18,0x18,0x18,0x00,0x18,0x3C},
                      {0x18,0x18,0x18,0x18,0x00,0x18,0x3C,0x7E},
                      {0x18,0x18,0x18,0x00,0x18,0x3C,0x7E,0x18},
                      {0x18,0x18,0x00,0x18,0x3C,0x7E,0x18,0x18},
                      {0x18,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18},
                      {0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x18}},// UP
                      
                      {{0x00,0x20,0x60,0xFE,0xFE,0x60,0x20,0x00},
                      {0x00,0x40,0xC0,0xFD,0xFD,0xC0,0x40,0x00},
                      {0x00,0x80,0x81,0xFB,0xFB,0x81,0x80,0x00},
                      {0x00,0x01,0x03,0xF7,0xF7,0x03,0x01,0x00},
                      {0x00,0x02,0x06,0xEF,0xEF,0x06,0x02,0x00},
                      {0x00,0x04,0x0C,0xDF,0xDF,0x0C,0x04,0x00},
                      {0x00,0x08,0x18,0xBF,0xBF,0x18,0x08,0x00},
                      {0x00,0x10,0x30,0x7F,0x7F,0x30,0x10,0x00}}, // LEFT
                      
                      {{0x00,0x04,0x06,0x7F,0x7F,0x06,0x04,0x00},
                      {0x00,0x02,0x03,0xBF,0xBF,0x03,0x02,0x00},
                      {0x00,0x01,0x81,0xDF,0xDF,0x81,0x01,0x00},
                      {0x00,0x80,0xC0,0xEF,0xEF,0xC0,0x80,0x00},
                      {0x00,0x40,0x60,0xF7,0xF7,0x60,0x40,0x00},
                      {0x00,0x20,0x30,0xFB,0xFB,0x30,0x20,0x00},
                      {0x00,0x10,0x18,0xFD,0xFD,0x18,0x10,0x00},
                      {0x00,0x08,0x0C,0xFE,0xFE,0x0C,0x08,0x00}}// RIGHT
                      }; 

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_FORWARD, INPUT_PULLUP);

  lc.shutdown(0, false);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);

  lcd.init();
  lcd.backlight();
  
  cli();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 12000;
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  sei();
}

void printByte(byte object[]) {
  for (int i = 0; i < 8; ++i) {
    lc.setRow(0, i, object[i]);
  }
}

void printMessage(String message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message);
}

ISR(TIMER1_COMPA_vect) {
  // how quick to change the state
  offset += 1;
  offset %= 8;

  printByte(arrows[directionToMove][offset]);

  int flickeringValue = analogRead(A1);

  flickeringValue = max(lower, flickeringValue);
  flickeringValue = min(flickeringValue, higher);

  OCR1A = lowestTick + valuePerSlide * flickeringValue;
}

void readButtonInput() {
  int leftSignal, rightSignal, forwardSignal;

  leftSignal = digitalRead(BUTTON_LEFT);
  rightSignal = digitalRead(BUTTON_RIGHT);
  forwardSignal = digitalRead(BUTTON_FORWARD);

  if (leftSignal == LOW && directionToMove != LEFT) {
    directionToMove = LEFT;
    hasChanged = true;
  } else if (rightSignal == LOW && directionToMove != RIGHT) {
    directionToMove = RIGHT;
    hasChanged = true;
  } else if (forwardSignal == LOW && directionToMove != FORWARD) {
    directionToMove = FORWARD;
    hasChanged = true;
  }
}

void loop()
{
  readButtonInput();

  if (hasChanged) {
    printMessage(messages[directionToMove]);
    hasChanged = false;
  }
  
}
