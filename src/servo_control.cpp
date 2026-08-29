#include "servo_control.h"
#include "config.h"
#include <Arduino.h>

void initServo() {
  pinMode(SERVO_PIN, OUTPUT);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(SERVO_PIN, PWM_CHANNEL);
  Serial.println("Servo hardware initialized");
}

void setServoPulse(int pulseUs) {
  // ЖЕСТКИЕ ограничения для серво 180°
  pulseUs = constrain(pulseUs, 500, 2500);
  
  int value = map(pulseUs, 0, 20000, 0, (1 << PWM_RESOLUTION) - 1);
  ledcWrite(PWM_CHANNEL, value);
  
  // Отладка - показываем изменения
  static int last_logged = 0;
  if (abs(pulseUs - last_logged) > 15) {
    last_logged = pulseUs;
    Serial.printf("Servo PWM: %d us\n", pulseUs);
  }
}

void updateHorizonControl(float current_pitch, int16_t rc_value, int servo_center, int max_angle, float angle_multiplier, bool reverse_servo, uint32_t last_attitude_time) {
  // Быстрая проверка на валидность данных
  if (millis() - last_attitude_time > 500) { // Если данные старше 500 мс
    setServoPulse(servo_center);
    return;
  }
  
  // Отладка входных данных
  static uint32_t last_input_debug = 0;
  if (millis() - last_input_debug > 2000) {
    last_input_debug = millis();
    Serial.printf("Input - Pitch: %.2f°, RC: %d\n", current_pitch, rc_value);
  }
  
  float offset_percent = 0;
  
  // Проверим валидность RC данных
  if (rc_value > 600 && rc_value < 2400) {
    offset_percent = map(rc_value, 600, 2400, -100, 100);
  }
  
  // БАЛАНС чувствительности: компенсация pitch + RC управление
  float target_angle = -current_pitch * 1 + (offset_percent * 1);
  target_angle = constrain(target_angle, -max_angle, max_angle);
  
  // ПРИМЕНЯЕМ МНОЖИТЕЛЬ УГЛА
  target_angle *= angle_multiplier;
  
  // ОБРАТНОЕ НАПРАВЛЕНИЕ
  if (reverse_servo) {
    target_angle = -target_angle;
  }
  
  // КОНВЕРТАЦИЯ для серво 180° (500-2500 мкс):
  // 11.11 мкс/градус
  float pwm_per_degree = 11.11;
  
  // Рассчитываем PWM
  int target_pwm = servo_center + (target_angle * pwm_per_degree);
  
  // Ограничиваем диапазон
  int max_deviation_pwm = max_angle * pwm_per_degree;
  target_pwm = constrain(target_pwm, 
                        servo_center - max_deviation_pwm,
                        servo_center + max_deviation_pwm);
  
  // Жесткие ограничения
  target_pwm = constrain(target_pwm, 500, 2500);
  
  // Отладка расчетов
  if (millis() - last_input_debug > 2000) {
    Serial.printf("Calc - Angle: %.2f°, PWM: %d us\n", target_angle, target_pwm);
  }
  
  setServoPulse(target_pwm);
}
