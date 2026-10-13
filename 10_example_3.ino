/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://ardui=nogetstarted.com/faq/how-to-control-speed-of-servo-motor
 */



#include <Servo.h>

#define PIN_SERVO 10
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_LED   9

#define MODE 2          // 1: sigmoid, 2: smootherstep
const int  CLOSED_ANGLE = 30;  
const int  OPEN_ANGLE   = 90;  
const unsigned long MOVING_TIME = 1500;  
const float DETECT_CM   = 15.0;          
const unsigned long CLEAR_HOLD = 1500;   
const float SIGMOID_K   = 10.0;          

Servo myServo;

enum State { CLOSED, OPENING, OPEN, CLOSING };
State state = CLOSED;

unsigned long moveStartTime = 0;
unsigned long lastSeenTime = 0;
int fromAngle = CLOSED_ANGLE;
int toAngle = CLOSED_ANGLE;
int currentAngle = CLOSED_ANGLE;


float sigmoidEase(float t) {
  float raw  = 1.0 / (1.0 + exp(-SIGMOID_K * (t - 0.5)));
  float low  = 1.0 / (1.0 + exp( SIGMOID_K * 0.5));
  float high = 1.0 / (1.0 + exp(-SIGMOID_K * 0.5));
  return (raw - low) / (high - low);
}


float smootherstep(float t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float ease(float t) {
#if MODE == 1
  return sigmoidEase(t);
#else
  return smootherstep(t);
#endif
}


float readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long dur = pulseIn(PIN_ECHO, HIGH, 25000UL);  
  if (dur == 0) return 999.0;
  return dur * 0.034 / 2.0;
}

void startMove(int target, State next) {
  fromAngle = currentAngle;
  toAngle = target;
  moveStartTime = millis();
  state = next;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED, OUTPUT);
  myServo.attach(PIN_SERVO);
  myServo.write(CLOSED_ANGLE);
  delay(500);
  Serial.println("ms,state,angle");  
}

void loop() {
  float dist = readDistanceCm();
  bool detected = (dist < DETECT_CM);
  unsigned long now = millis();
  if (detected) lastSeenTime = now;

  switch (state) {
    case CLOSED:
      if (detected) startMove(OPEN_ANGLE, OPENING);
      break;

    case OPENING:
    case CLOSING: {
      unsigned long progress = now - moveStartTime;
      if (progress >= MOVING_TIME) {
        currentAngle = toAngle;
        myServo.write(currentAngle);
        state = (state == OPENING) ? OPEN : CLOSED;
      } else {
        float t = (float)progress / MOVING_TIME;
        currentAngle = fromAngle + (int)((toAngle - fromAngle) * ease(t));
        myServo.write(currentAngle);
      }
      if (state == CLOSING && detected) startMove(OPEN_ANGLE, OPENING);
      break;
    }

    case OPEN:
      if (!detected && (now - lastSeenTime > CLEAR_HOLD)) {
        startMove(CLOSED_ANGLE, CLOSING);
      }
      break;
  }


  Serial.print(now);
  Serial.print(',');
  Serial.print(state);
  Serial.print(',');
  Serial.println(currentAngle);
  delay(15);  
}
