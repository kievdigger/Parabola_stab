#include "web_server.h"
#include "config.h"
#include <Arduino.h>

// Глобальные переменные
WebServer server(80);
Config g_config;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Horizon Controller</title>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;margin:20px;background:#f5f5f5;}";
  html += ".container{max-width:600px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
  html += "h1{text-align:center;color:#333;} h3{color:#555;}";
  html += ".form-group{margin:15px 0;} label{display:block;margin-bottom:5px;font-weight:bold;}";
  html += "input,select{width:100%;padding:10px;border:1px solid #ddd;border-radius:5px;box-sizing:border-box;}";
  html += "button{background:#007cba;color:white;padding:12px 20px;border:none;border-radius:5px;cursor:pointer;width:100%;font-size:16px;}";
  html += "button:hover{background:#005a87;} .status{background:#e7f3ff;padding:15px;border-radius:5px;margin:20px 0;}"; 
  html += ".data-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:10px;}";
  html += ".data-item{background:#f9f9f9;padding:10px;border-radius:5px;}</style></head><body>";
  html += "<div class='container'><h1>&#127922; Horizon Controller</h1>";
  
  html += "<div class='status'><h3>Status</h3><div class='data-grid'>";
  html += "<div class='data-item'>WiFi: Connected</div>";
  html += "<div class='data-item'>IP: 192.168.4.1</div>";
  html += "<div class='data-item'>Status: Ready</div>";
  html += "</div></div>";

  html += "<form method='POST' action='/save'>";
  html += "<h3>Configuration</h3>";
  
  // Основные настройки
  html += "<div class='form-group'><label for='channel'>Horizon Channel (1-18):</label>";
  html += "<input type='number' id='channel' name='channel' value='" + String(g_config.horizon_channel) + "' min='1' max='18' required></div>";
  
  html += "<div class='form-group'><label for='center'>Servo Center (us):</label>";
  html += "<input type='number' id='center' name='center' value='" + String(g_config.servo_center) + "' min='500' max='2500' required></div>";
  
  html += "<div class='form-group'><label for='angle'>Max Angle (degrees):</label>";
  html += "<input type='number' id='angle' name='angle' value='" + String(g_config.max_angle) + "' min='5' max='90' required></div>";
  
  // НОВЫЕ настройки серво
  html += "<h4>Servo PWM Range</h4>";
  html += "<div class='form-group'><label for='min_pwm'>Min PWM (us):</label>";
  html += "<input type='number' id='min_pwm' name='min_pwm' value='" + String(g_config.servo_min_pwm) + "' min='500' max='2500' required></div>";
  
  html += "<div class='form-group'><label for='max_pwm'>Max PWM (us):</label>";
  html += "<input type='number' id='max_pwm' name='max_pwm' value='" + String(g_config.servo_max_pwm) + "' min='500' max='2500' required></div>";
  
  // НОВОЕ: Множитель угла поворота
  html += "<h4>Angle Control</h4>";
  html += "<div class='form-group'><label for='multiplier'>Angle Multiplier:</label>";
  html += "<input type='number' id='multiplier' name='multiplier' value='" + String(g_config.angle_multiplier, 2) + "' min='0.1' max='5.0' step='0.1' required></div>";
  
  // НОВОЕ: Обратное направление
  html += "<div class='form-group'><label for='reverse'>Reverse Servo Direction:</label>";
  html += "<select id='reverse' name='reverse'>";
  html += "<option value='0'" + String((g_config.reverse_servo == false) ? " selected" : "") + ">Normal</option>";
  html += "<option value='1'" + String((g_config.reverse_servo == true) ? " selected" : "") + ">Reversed</option>";
  html += "</select></div>";
  
  // MAVLink настройки
  html += "<h4>MAVLink Settings</h4>";
  html += "<div class='form-group'><label for='baud'>MAVLink Baud:</label>";
  html += "<select id='baud' name='baud'>";
  html += "<option value='9600'" + String((g_config.mavlink_baud == 9600) ? " selected" : "") + ">9600</option>";
  html += "<option value='19200'" + String((g_config.mavlink_baud == 19200) ? " selected" : "") + ">19200</option>";
  html += "<option value='38400'" + String((g_config.mavlink_baud == 38400) ? " selected" : "") + ">38400</option>";
  html += "<option value='57600'" + String((g_config.mavlink_baud == 57600) ? " selected" : "") + ">57600</option>";
  html += "<option value='115200'" + String((g_config.mavlink_baud == 115200) ? " selected" : "") + ">115200</option>";
  html += "</select></div>";
  
  html += "<button type='submit'>&#128190; Save Settings</button></form>";
  html += "<p style='text-align:center;margin-top:20px;'><small>After saving, device will restart</small></p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.method() == HTTP_POST) {
    // Получаем значения из формы
    g_config.horizon_channel = server.arg("channel").toInt();
    g_config.servo_center = server.arg("center").toInt();
    g_config.max_angle = server.arg("angle").toInt();
    g_config.servo_min_pwm = server.arg("min_pwm").toInt();  // Новое
    g_config.servo_max_pwm = server.arg("max_pwm").toInt();  // Новое
    g_config.angle_multiplier = server.arg("multiplier").toFloat(); // Новое
    g_config.reverse_servo = server.arg("reverse").toInt() != 0; // Новое
    g_config.mavlink_baud = server.arg("baud").toInt();
    
    // Проверяем логику значений
    if (g_config.servo_min_pwm >= g_config.servo_max_pwm) {
      g_config.servo_min_pwm = 1000;
      g_config.servo_max_pwm = 2000;
    }
    
    // Сохраняем в EEPROM
    saveConfig(g_config);
    
    // Отправляем страницу подтверждения
    String response = "<!DOCTYPE html><html><head>";
    response += "<title>Settings Saved</title>";
    response += "<meta http-equiv='refresh' content='3;url=/'>";
    response += "<style>body{font-family:sans-serif;text-align:center;margin-top:50px;background:#f5f5f5;}";
    response += ".container{max-width:400px;margin:0 auto;background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
    response += "h1{color:#4CAF50;}</style></head><body>";
    response += "<div class='container'><h1>&#10004; Settings Saved!</h1>";
    response += "<p>Device will restart in 3 seconds...</p></div></body></html>";
    
    server.send(200, "text/html", response);
    
    // Перезагружаем устройство через 1 секунду
    delay(1000);
    ESP.restart();
  }
}

void setupWebPages() {
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  Serial.println("Web pages configured");
}

void initWebServer() {
  // Загружаем конфигурацию из EEPROM
  loadConfig(g_config);
  
  // Настраиваем WiFi точку доступа
  WiFi.mode(WIFI_AP);
  WiFi.softAP("HorizonCtrl", "12345678");
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  
  // Запускаем сервер
  server.begin();
  
  Serial.println("WiFi AP started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Web server ready");
}
