#include <Servo.h>

// Arduino pin assignment
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10

// configurable parameters for sonar
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 180.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

// configurable parameters for Servo
#define _DUTY_MIN 553 // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counter-clockwise position (180 degree)

// 실험 1: _SERVO_SPEED 3,   _MOVE_ANGLE 180  -> 60초 동안 180도 등속 이동
// 실험 2: _SERVO_SPEED 0.3, _MOVE_ANGLE 90   -> 300초 동안 90도 등속 이동
#define _SERVO_SPEED 0.3   // servo angular speed (unit: degree/sec)
#define _MOVE_ANGLE  90 // 한 방향 이동 각도 (unit: degree)

#define _POS_START _DUTY_MIN
#define _POS_END   (_DUTY_MIN + (_DUTY_MAX - _DUTY_MIN) * (_MOVE_ANGLE / 180.0))

// Loop Interval
#define INTERVAL 20     // servo update interval (unit: msec)

// global variables
unsigned long last_sampling_time; // unit: msec

Servo myservo;

float duty_change_per_interval; // maximum duty difference per interval
float duty_target;    // Target duty time
float duty_curr;      // Current duty time

int toggle_interval, toggle_interval_cnt;

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);

  duty_target = duty_curr = _POS_START;
  myservo.writeMicroseconds((int)duty_curr);

  Serial.begin(57600);

  duty_change_per_interval =
    (float)(_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.0) * (INTERVAL / 1000.0);

  Serial.print("duty_change_per_interval:");
  Serial.println(duty_change_per_interval, 4);

  toggle_interval = (_MOVE_ANGLE / (float)_SERVO_SPEED) * 1000 / INTERVAL;
  toggle_interval_cnt = toggle_interval;

  last_sampling_time = 0;
}

void loop() {
  float dist_raw;

  if (millis() < (last_sampling_time + INTERVAL))
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if (duty_target > duty_curr) {
    duty_curr += duty_change_per_interval;
    if (duty_curr > duty_target)
      duty_curr = duty_target;
  } else {
    duty_curr -= duty_change_per_interval;
    if (duty_curr < duty_target)
      duty_curr = duty_target;
  }

  myservo.writeMicroseconds((int)(duty_curr + 0.5));

  Serial.print("Min:");     Serial.print(_DUTY_MIN);
  Serial.print(",target:"); Serial.print(duty_target, 1);
  Serial.print(",curr:");   Serial.print(duty_curr, 1);
  Serial.print(",dist:");   Serial.print(dist_raw, 0);
  Serial.print(",Max:");    Serial.println(_DUTY_MAX);

  if (toggle_interval_cnt >= toggle_interval) {
    toggle_interval_cnt = 0;
    if (duty_target == _POS_START)
      duty_target = _POS_END;
    else
      duty_target = _POS_START;
  } else {
    toggle_interval_cnt++;
  }

  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
