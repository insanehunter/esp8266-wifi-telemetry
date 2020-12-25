/*
  Распиновка переключателя режимов работы:
    Upload to ESP8266 (WiFi)              0 0 0 0 1 1 1 0
    Upload to ATmega328 (Thermo)          0 0 1 1 0 0 0 0
    Interconnect ESP8266 and ATmega328    1 1 0 0 0 0 0 0
*/
#include <ESP8266HTTPClient.h>
#include <CircularBuffer.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "debug_print.h"
#include "wifi.h"

typedef struct {
  unsigned long epoch;
  unsigned long clock;
  float median_temp;
} Measurement;

const int max_measurements = 1000;
const int min_measurements_per_batch = 5;

CircularBuffer<Measurement, max_measurements> measurements;

bool wifi_connection_restored = true;

const int report_payload_buffer_size = 300;
char *report_payload_buffer;

void setup()
{
  Serial.begin(57600);
  wifi_connect(wifi_network_name, wifi_network_password);
  report_payload_buffer = (char *)malloc(report_payload_buffer_size);
}

void loop() {
  // Reading measurement.
  if (!Serial.available()) {
    return;
  }
  debug_print("Received new measurement\n");
  String json = Serial.readString();
  StaticJsonDocument<128> measurement_doc;
  deserializeJson(measurement_doc, json);

  Measurement measurement = {measurement_doc["epoch"], measurement_doc["clock"], measurement_doc["median_temp"]};
  measurements.push(measurement);

  // Checking wifi connection.
  if (measurements.size() < min_measurements_per_batch) {
    return;
  }
  if (!wifi_connected()) {
    wifi_connection_restored = false;
    debug_print("Cannot send data: wifi not connected\n");
    return;
  }
  if (wifi_connection_restored) {
    wifi_print_ip_address();
    wifi_connection_restored = true;
  }

  // Building JSON payload.
  DynamicJsonDocument payload_doc(report_payload_buffer_size);
  JsonObject root = payload_doc.to<JsonObject>();
  JsonObject median_temps = root.createNestedObject("median_temps");
  using index_t = decltype(measurements)::index_t;
  char key_buffer[22];
  for (index_t i = 0; i < measurements.size(); ++i) {
    const Measurement &m = measurements[i];
    snprintf(key_buffer, sizeof(key_buffer) / sizeof(key_buffer[0]), "%lu_%010lu", m.epoch, m.clock);
    median_temps[key_buffer] = m.median_temp;
  }
  size_t payload_size = serializeJson(payload_doc, report_payload_buffer, report_payload_buffer_size);
  debug_print("Payload: ");
  debug_print(report_payload_buffer);
  debug_print("\n");
  
  // Sending measurements.
  HTTPClient http;
  http.begin(report_measurements_url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT((const uint8_t *)report_payload_buffer, payload_size);
  debug_print("Response code: ");
  debug_print(String(httpCode));
  debug_print("\n");
  if (httpCode < 0) {
    debug_print(http.errorToString(httpCode));
    debug_print("\n");
  }
  else if (httpCode == 200) {
    debug_print("Measurements sent!\n");
    measurements.clear();
  }
  http.end();
}
