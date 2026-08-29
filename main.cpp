#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "servo_control.h"
#include "mavlink_handler.h"
#include "web_server.h"

// Global variables
TelemetryData telemetry;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n=== Horizon Controller ===");
  
  // Initialize servo
  initServo();
  
  // Загружаем конфигурацию перед использованием
  loadConfig(g_config);
  setServoPulse(g_config.servo_center);
  Serial.println("Servo initialized");
  
  // Initialize MAVLink
  initMavlink(g_config.mavlink_baud);
  Serial.println("MAVLink initialized");
  
  // Initialize WiFi and Web server
  initWebServer();
  setupWebPages();
  Serial.println("Web server started");
  
  // Initialize telemetry data
  memset(&telemetry, 0, sizeof(telemetry));
  
  Serial.println("Ready!");
  Serial.println("==================");
}

void loop() {
  // Handle web requests
  server.handleClient();
  
  // Process MAVLink telemetry
  processTelemetry(telemetry);
  
  // Send heartbeat periodically
  static uint32_t last_heartbeat = 0;
  if (millis() - last_heartbeat > 1000) {
    last_heartbeat = millis();
    sendHeartbeat();
    
    // Включаем поток телеметрии, если есть системный ID
    if (telemetry.target_system > 0 && telemetry.target_component > 0) {
      // Отправляем команду только один раз при запуске
      static bool streaming_enabled = false;
      if (!streaming_enabled) {
        enableTelemetryStreaming(telemetry.target_system, telemetry.target_component);
        streaming_enabled = true;
      }
    }
  }
  
  // Update servo control
  static uint32_t last_servo_update = 0;
  if (millis() - last_servo_update > 20) { // Обновляем каждые 20 мс
    last_servo_update = millis();
    
    // Проверяем наличие актуальных данных
    if (telemetry.attitude_available && (millis() - telemetry.last_attitude_time) < 1000) {
      int16_t rc_value = telemetry.rc_channels[g_config.horizon_channel - 1];
      updateHorizonControl(telemetry.current_pitch, rc_value, g_config.servo_center, g_config.max_angle, g_config.angle_multiplier, g_config.reverse_servo, telemetry.last_attitude_time);
    } else {
      setServoPulse(g_config.servo_center);
    }
  }
  
  // Show stats every 5 seconds
  static uint32_t last_stats = 0;
  if (millis() - last_stats > 5000) {
    last_stats = millis();
    Serial.printf("Stats - Attitude age: %d ms, Last RC: %d\n", 
                  millis() - telemetry.last_attitude_time,
                  telemetry.rc_channels[g_config.horizon_channel - 1]);
  }
  
  // Минимальная задержка для стабильной работы
  delay(1);
}
