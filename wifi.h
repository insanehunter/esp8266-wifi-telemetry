#pragma once

bool wifi_connected();
void wifi_connect(const char *wifi_network_name, const char *wifi_network_password);
void wifi_print_ip_address();
