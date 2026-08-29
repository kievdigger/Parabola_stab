#include "config.h"

void loadConfig(Config& config) {
  EEPROM.begin(EEPROM_SIZE);
  uint32_t magic;
  EEPROM.get(ADDR_MAGIC, magic);
  
  Serial.printf("Loading config, magic: 0x%X\n", magic);
  
  if (magic == MAGIC_NUMBER) {
    config.wifi_ssid = EEPROM.readString(ADDR_WIFI_SSID);
    config.wifi_password = EEPROM.readString(ADDR_WIFI_PASS);
    EEPROM.get(ADDR_MAVLINK_BAUD, config.mavlink_baud);
    EEPROM.get(ADDR_HORIZON_CHANNEL, config.horizon_channel);
    EEPROM.get(ADDR_SERVO_CENTER, config.servo_center);
    EEPROM.get(ADDR_MAX_ANGLE, config.max_angle);
    EEPROM.get(ADDR_SERVO_MIN_PWM, config.servo_min_pwm);
    EEPROM.get(ADDR_SERVO_MAX_PWM, config.servo_max_pwm);
    EEPROM.get(ADDR_ANGLE_MULTIPLIER, config.angle_multiplier);
    EEPROM.get(ADDR_REVERSE_SERVO, config.reverse_servo);
    
    Serial.printf("Loaded config from EEPROM\n");
  } else {
    // Use defaults
    config.wifi_ssid = DEFAULT_WIFI_SSID;
    config.wifi_password = DEFAULT_WIFI_PASS;
    config.mavlink_baud = DEFAULT_MAVLINK_BAUD;
    config.horizon_channel = DEFAULT_HORIZON_CHANNEL;
    config.servo_center = DEFAULT_SERVO_CENTER;
    config.max_angle = DEFAULT_MAX_ANGLE;
    config.servo_min_pwm = DEFAULT_SERVO_MIN_PWM;
    config.servo_max_pwm = DEFAULT_SERVO_MAX_PWM;
    config.angle_multiplier = DEFAULT_ANGLE_MULTIPLIER;
    config.reverse_servo = DEFAULT_REVERSE_SERVO;
    
    Serial.printf("Using default config\n");
  }
  
  // Проверим и исправим некорректные значения
  if (config.servo_min_pwm <= 0 || config.servo_min_pwm > 2500) {
    config.servo_min_pwm = DEFAULT_SERVO_MIN_PWM;
  }
  if (config.servo_max_pwm <= 0 || config.servo_max_pwm > 2500) {
    config.servo_max_pwm = DEFAULT_SERVO_MAX_PWM;
  }
  if (config.servo_center <= 0 || config.servo_center > 2500) {
    config.servo_center = DEFAULT_SERVO_CENTER;
  }
  if (config.angle_multiplier <= 0) {
    config.angle_multiplier = DEFAULT_ANGLE_MULTIPLIER;
  }
  
  Serial.printf("Final config - Min: %d, Max: %d, Center: %d, Multiplier: %.2f, Reverse: %s\n", 
                config.servo_min_pwm, config.servo_max_pwm, config.servo_center, 
                config.angle_multiplier, config.reverse_servo ? "true" : "false");
  
  EEPROM.end();
}

void saveConfig(const Config& config) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(ADDR_MAGIC, MAGIC_NUMBER);
  EEPROM.writeString(ADDR_WIFI_SSID, config.wifi_ssid);
  EEPROM.writeString(ADDR_WIFI_PASS, config.wifi_password);
  EEPROM.put(ADDR_MAVLINK_BAUD, config.mavlink_baud);
  EEPROM.put(ADDR_HORIZON_CHANNEL, config.horizon_channel);
  EEPROM.put(ADDR_SERVO_CENTER, config.servo_center);
  EEPROM.put(ADDR_MAX_ANGLE, config.max_angle);
  EEPROM.put(ADDR_SERVO_MIN_PWM, config.servo_min_pwm);
  EEPROM.put(ADDR_SERVO_MAX_PWM, config.servo_max_pwm);
  EEPROM.put(ADDR_ANGLE_MULTIPLIER, config.angle_multiplier);
  EEPROM.put(ADDR_REVERSE_SERVO, config.reverse_servo);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.println("Config saved to EEPROM");
}
