#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>

// Function declarations
void initServo();
void setServoPulse(int pulseUs);
void updateHorizonControl(float current_pitch, int16_t rc_value, int servo_center, int max_angle, float angle_multiplier, bool reverse_servo, uint32_t last_attitude_time);

#endif
