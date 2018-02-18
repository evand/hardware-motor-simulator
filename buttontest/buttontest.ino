/*
 * Test out push button, scroll switch, and action LED.
 * 
 * LED goes on when you push the button.
 * LED blinks slow when scroll is down
 * LED blinks fast when scroll is up
 */

const int LED = 13;
const int ACTION = A3;
const int SCROLL = A6;
const int BLINK_SLOW = 500;
const int BLINK_FAST = 100;

static long int last_blink_time;
static long int loop_time;
static int blink_period;
static int blink_state;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(ACTION, INPUT_PULLUP);
  pinMode(SCROLL, INPUT);

  blink_state = 0;
  blink_period = 0;
}

void loop() {
  int v;

  loop_time = millis();

  // If the button is pushed, turn on the LED
  if (digitalRead(ACTION) == 0) {
    digitalWrite(LED, 1);
    return;
  }

  // If the scroll switch is pushed, blink the LED
  v = analogRead(SCROLL);
  if (v < 10)
    blink_period = BLINK_SLOW;
  else if (v > 1000)
    blink_period = BLINK_FAST;
  else {
    digitalWrite(LED, 0);
    last_blink_time = loop_time;
    blink_state = 1;
    return;
  }

  if (loop_time > last_blink_time + blink_period) {
     blink_state = 1 - blink_state;
     last_blink_time = loop_time;
  }
  digitalWrite(LED, blink_state);
}
