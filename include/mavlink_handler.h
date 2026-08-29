#ifndef MAVLINK_HANDLER_H
#define MAVLINK_HANDLER_H

#include <Arduino.h>
#include <../lib/mavlink/common/mavlink.h>

// Structure to hold telemetry data
struct TelemetryData {
  float current_roll;
  float current_pitch;
  float current_yaw;
  int16_t rc_channels[18];
  uint32_t last_attitude_time;
  uint32_t last_rc_time;
  uint32_t last_heartbeat_time;
  uint8_t target_system;
  uint8_t target_component;
  uint32_t mavlink_msg_count;
  uint32_t mavlink_parse_errors;
  bool attitude_available;    // Флаг наличия данных о姿态
  bool rc_available;          // Флаг наличия данных о RC
};

// Function declarations
void initMavlink(int baud_rate);
void processTelemetry(TelemetryData& telemetry);
void sendHeartbeat();
void requestMessages(uint8_t target_system, uint8_t target_component);
void handleMavlinkMessage(mavlink_message_t* msg, TelemetryData& telemetry);
void enableTelemetryStreaming(uint8_t target_system, uint8_t target_component);

#endif
