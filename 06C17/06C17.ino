const int LED_PIN = 9;
int period_us = 1000;
int duty_percent = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void set_period(int period) {
  period_us = period;
}

void set_duty(int duty) {
  duty_percent = duty;
}

void custom_pwm_pulse() {
  long high_time = (period_us * duty_percent) / 100;
  long low_time = period_us - high_time;

  if (high_time > 0) {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(high_time);
  }
  if (low_time > 0) {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(low_time);
  }
}

void loop() {
  set_period(1000);

  for (int d = 0; d <= 100; d++) {
    set_duty(d);
    for (int i = 0; i < 10; i++) {
      custom_pwm_pulse();
    }
  }

  for (int d = 100; d >= 0; d--) {
    set_duty(d);
    for (int i = 0; i < 10; i++) {
      custom_pwm_pulse();
    }
  }
}셀레나  [오전 9:51]
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
  last_sampling_time = millis();
}

void loop() {
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  last_sampling_time += INTERVAL;

  float distance = USS_measure(PIN_TRIG, PIN_ECHO);
  int pwm_value = 255;

  if (distance >= _DIST_MIN && distance <= _DIST_MAX) {
    float duty = 0.0;
    if (distance <= 200.0) {
      duty = (distance - 100.0) / 100.0;
    } else {
      duty = (300.0 - distance) / 100.0;
    }
    pwm_value = 255 - (int)(duty * 255.0);
  } else {
    pwm_value = 255;
  }

  analogWrite(PIN_LED, pwm_value);

  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",pwm:");       Serial.print(pwm_value);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
