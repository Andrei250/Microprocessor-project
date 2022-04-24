#define FORWARD 0
#define LEFT 1
#define RIGHT 2
#define STATE_ON 1
#define STATE_OFF 0

unsigned const int LED_LEFT = 2;
unsigned const int LED_RIGHT = 6;
unsigned const int LED_FORWARD = 7;
unsigned const int BUTTON_LEFT = 3;
unsigned const int BUTTON_RIGHT = 4;
unsigned const int BUTTON_FORWARD = 5;
unsigned int directionToMove = FORWARD; // By default is set to forward
unsigned int flickeringState = STATE_ON;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_FORWARD, INPUT_PULLUP);

  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_FORWARD, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  cli();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 31249;            // compare match register 16MHz/256/2Hz-1
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  sei();
}

void turnOffMatrix() {
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_FORWARD, LOW);
}

void printMatrix() {
  if (flickeringState == STATE_ON) {
    if (directionToMove == LEFT) {
      digitalWrite(LED_LEFT, HIGH);
    } else if (directionToMove == RIGHT) {
      digitalWrite(LED_RIGHT, HIGH);
    } else if (directionToMove == FORWARD) {
      digitalWrite(LED_FORWARD, HIGH);
    }
  } else {
    turnOffMatrix();
  }
  
}

ISR(TIMER1_COMPA_vect) {
//  Serial.println(flickeringState, DEC);

  // how quick to change the state
  OCR1A += 1000;

  flickeringState = 1 - flickeringState;
  printMatrix();
}

void readButtonInput() {
  int leftSignal, rightSignal, forwardSignal;

  leftSignal = digitalRead(BUTTON_LEFT);
  rightSignal = digitalRead(BUTTON_RIGHT);
  forwardSignal = digitalRead(BUTTON_FORWARD);

  if (leftSignal == LOW) {
    directionToMove = LEFT;
    turnOffMatrix();
  } else if (rightSignal == LOW) {
    directionToMove = RIGHT;
    turnOffMatrix();
  } else if (forwardSignal == LOW) {
    directionToMove = FORWARD;
    turnOffMatrix();
  }
}

void loop()
{
  readButtonInput();
}
