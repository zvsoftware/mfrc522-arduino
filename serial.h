#include <ArduinoJson.h>

StaticJsonDocument<200> listen_for_commands() {
  int wait = 200;
  int i = 0;

  // Read the incoming data
  String json = Serial.readStringUntil('\n');

  // Wait for incoming data on the serial port
  while (json.charAt(0) != '{' && i < wait) {
    delay(100);
    i += 100;
  }

  // Parse the JSON data
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);

  // // Check for parsing errors
  // if (error) {
  //   Serial.print("Error: ");
  //   Serial.println(error.c_str());
  //   return;
  // }

  return doc;
}