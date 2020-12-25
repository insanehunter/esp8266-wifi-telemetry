#include <ESP8266WiFi.h>
#include "debug_print.h"
#include "wifi.h"

bool wifi_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifi_connect(const char *wifi_network_name, const char *wifi_network_password) {
  WiFi.begin(wifi_network_name, wifi_network_password);

  debug_print("Connecting");
  while (!wifi_connected())
  {
    delay(500);
    debug_print(".");
  }
  debug_print("\n");
  wifi_print_ip_address();
}

void wifi_print_ip_address() {
  debug_print("Connected, IP address: ");
  debug_print(WiFi.localIP());
  debug_print("\n");
}
