#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "config.h"

// Глобальные переменные
extern WebServer server;
extern Config g_config;

// Function declarations
void initWebServer();
void setupWebPages();
void handleRoot();
void handleSave();

#endif
