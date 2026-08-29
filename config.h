#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

// Default settings
#define DEFAULT_WIFI_SSID "HorizonCtrl"
#define DEFAULT_WIFI_PASS "12345678"
#define DEFAULT_MAVLINK_BAUD 57600
#define DEFAULT_HORIZON_CHANNEL 10
#define DEFAULT_SERVO_CENTER 1500
#define DEFAULT_MAX_ANGLE 15
#define DEFAULT_SERVO_MIN_PWM 1000  // Минимальный PWM
#define DEFAULT_SERVO_MAX_PWM 2000  // Максимальный PWM
#define DEFAULT_ANGLE_MULTIPLIER 1.0 // Множитель угла поворота
#define DEFAULT_REVERSE_SERVO false  // Обратное направление по умолчанию

// Параметры скорости обработки
#define MAVLINK_UPDATE_INTERVAL 50    // Интервал обновления MAVLink (мс)
#define SERVO_UPDATE_INTERVAL 20      // Интервал обновления серво (мс)

// Pin definitions
#define SERVO_PIN 4
#define MAVLINK_RX 20
#define MAVLINK_TX 21

// Servo parameters
#define PWM_CHANNEL 0
#define PWM_FREQ 50
#define PWM_RESOLUTION 10

// EEPROM addresses
#define EEPROM_SIZE 256
#define ADDR_MAGIC 0
#define ADDR_WIFI_SSID 4
#define ADDR_WIFI_PASS 36
#define ADDR_MAVLINK_BAUD 68
#define ADDR_HORIZON_CHANNEL 72
#define ADDR_SERVO_CENTER 76
#define ADDR_MAX_ANGLE 80
#define ADDR_SERVO_MIN_PWM 84
#define ADDR_SERVO_MAX_PWM 88
#define ADDR_ANGLE_MULTIPLIER 92 // Новое поле
#define ADDR_REVERSE_SERVO 96 // Новое поле - обратное направление
#define MAGIC_NUMBER 0x12345678

// Configuration structure
struct Config {
  String wifi_ssid;
  String wifi_password;
  int mavlink_baud;
  int horizon_channel;
  int servo_center;
  int max_angle;
  int servo_min_pwm;  // Новое поле
  int servo_max_pwm;  // Новое поле
  float angle_multiplier; // Новое поле - множитель угла поворота
  bool reverse_servo; // Новое поле - обратное направление
};

// Function declarations
void loadConfig(Config& config);
void saveConfig(const Config& config);

#endif
