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
}
