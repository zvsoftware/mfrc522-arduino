#include <ArduinoJson.h>

StaticJsonDocument<200> doc;
int wait;
int i;
StaticJsonDocument<200> listen_for_commands() {
  wait = 300;
  i = 0;
  // Read the incoming data
  String json = Serial.readStringUntil('\n');

  // Wait for incoming data on the serial port
  while (json.charAt(0) != '{' && i < wait) {
    delay(100);
    i += 100;
  }

  // Parse the JSON data
  DeserializationError error = deserializeJson(doc, json);
  return doc;
}