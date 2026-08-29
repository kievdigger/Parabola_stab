#include "mavlink_handler.h"
#include "config.h"
#include <Arduino.h>
#include <web_server.h>

void initMavlink(int baud_rate) {
  Serial1.begin(baud_rate, SERIAL_8N1, MAVLINK_RX, MAVLINK_TX);
  Serial.printf("MAVLink started at %d baud\n", baud_rate);
}

float radiansToDegrees(float radians) {
  return radians * 180.0 / M_PI;
}

// Функция для включения потоковой передачи данных
void enableTelemetryStreaming(uint8_t target_system, uint8_t target_component) {
  if (target_system == 0 || target_component == 0) {
    return;
  }

  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  uint16_t len;

  Serial.printf("Enabling telemetry streaming from sys:%d comp:%d\n", target_system, target_component);

  // Включаем поток ATTITUDE с частотой 20 Гц (50 мс)
  mavlink_msg_command_long_pack(
    255, 0, &msg,
    target_system, target_component,
    MAV_CMD_SET_MESSAGE_INTERVAL,
    0, MAVLINK_MSG_ID_ATTITUDE, 50000, 0, 0, 0, 0, 0
  );
  len = mavlink_msg_to_send_buffer(buffer, &msg);
  Serial1.write(buffer, len);
  Serial.println("ATTITUDE streaming enabled");

  // Включаем поток RC_CHANNELS с частотой 20 Гц (50 мс)
  mavlink_msg_command_long_pack(
    255, 0, &msg,
    target_system, target_component,
    MAV_CMD_SET_MESSAGE_INTERVAL,
    0, MAVLINK_MSG_ID_RC_CHANNELS, 50000, 0, 0, 0, 0, 0
  );
  len = mavlink_msg_to_send_buffer(buffer, &msg);
  Serial1.write(buffer, len);
  Serial.println("RC_CHANNELS streaming enabled");
}

void requestMessages(uint8_t target_system, uint8_t target_component) {
  // Теперь мы не отправляем запросы, а просто включаем поток
  // Поток будет автоматически передаваться от полетника
  enableTelemetryStreaming(target_system, target_component);
}

void sendHeartbeat() {
  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  uint16_t len;

  mavlink_msg_heartbeat_pack(
    255, 0, &msg,
    MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, 0
  );

  len = mavlink_msg_to_send_buffer(buffer, &msg);
  Serial1.write(buffer, len);
}

void handleMavlinkMessage(mavlink_message_t* msg, TelemetryData& telemetry) {
  telemetry.mavlink_msg_count++;
  
  switch (msg->msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT: {
      mavlink_heartbeat_t heartbeat;
      mavlink_msg_heartbeat_decode(msg, &heartbeat);
      telemetry.target_system = msg->sysid;
      telemetry.target_component = msg->compid;
      telemetry.last_heartbeat_time = millis();
      break;
    }
    
    case MAVLINK_MSG_ID_ATTITUDE: {
      mavlink_attitude_t attitude;
      mavlink_msg_attitude_decode(msg, &attitude);
      telemetry.current_roll = radiansToDegrees(attitude.roll);
      telemetry.current_pitch = radiansToDegrees(attitude.pitch);
      telemetry.current_yaw = radiansToDegrees(attitude.yaw);
      telemetry.last_attitude_time = millis();
      telemetry.attitude_available = true;
      break;
    }

    case MAVLINK_MSG_ID_RC_CHANNELS: {
      mavlink_rc_channels_t rc;
      mavlink_msg_rc_channels_decode(msg, &rc);
      
      // Используем значение из конфигурации для определения канала
      int channel_index = g_config.horizon_channel - 1; // Преобразуем к индексу массива (0-17)
      if (channel_index >= 0 && channel_index < 18) {
        // Получаем значение нужного канала из MAVLink структуры
        switch(channel_index) {
          case 0: telemetry.rc_channels[channel_index] = rc.chan1_raw; break;
          case 1: telemetry.rc_channels[channel_index] = rc.chan2_raw; break;
          case 2: telemetry.rc_channels[channel_index] = rc.chan3_raw; break;
          case 3: telemetry.rc_channels[channel_index] = rc.chan4_raw; break;
          case 4: telemetry.rc_channels[channel_index] = rc.chan5_raw; break;
          case 5: telemetry.rc_channels[channel_index] = rc.chan6_raw; break;
          case 6: telemetry.rc_channels[channel_index] = rc.chan7_raw; break;
          case 7: telemetry.rc_channels[channel_index] = rc.chan8_raw; break;
          case 8: telemetry.rc_channels[channel_index] = rc.chan9_raw; break;
          case 9: telemetry.rc_channels[channel_index] = rc.chan10_raw; break;
          case 10: telemetry.rc_channels[channel_index] = rc.chan11_raw; break;
          case 11: telemetry.rc_channels[channel_index] = rc.chan12_raw; break;
          case 12: telemetry.rc_channels[channel_index] = rc.chan13_raw; break;
          case 13: telemetry.rc_channels[channel_index] = rc.chan14_raw; break;
          case 14: telemetry.rc_channels[channel_index] = rc.chan15_raw; break;
          case 15: telemetry.rc_channels[channel_index] = rc.chan16_raw; break;
          case 16: telemetry.rc_channels[channel_index] = rc.chan17_raw; break;
          case 17: telemetry.rc_channels[channel_index] = rc.chan18_raw; break;
        }
      }
      telemetry.last_rc_time = millis();
      telemetry.rc_available = true;
      break;
    }
  }
}

void processTelemetry(TelemetryData& telemetry) {
  static mavlink_message_t msg;
  static mavlink_status_t status;

  // Обрабатываем все доступные данные
  while (Serial1.available()) {
    uint8_t c = Serial1.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      handleMavlinkMessage(&msg, telemetry);
    } else {
      telemetry.mavlink_parse_errors++;
    }
  }
}
