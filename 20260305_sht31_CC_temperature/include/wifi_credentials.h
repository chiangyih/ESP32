#pragma once

#include <Arduino.h>

struct WiFiCredentials
{
  String ssid;
  String password;
};

namespace
{
constexpr uint8_t WIFI_XOR_KEY = 0x5A;
const uint8_t WIFI_SSID[] = {50, 50, 44, 41, 119, 51, 53, 46};
const uint8_t WIFI_PASS[] = {18, 50, 44, 41, 26, 111, 110, 19, 53, 46};

String decodeText(const uint8_t *data, size_t length)
{
  String text;
  text.reserve(length);

  for (size_t i = 0; i < length; ++i)
  {
    text += char(data[i] ^ WIFI_XOR_KEY);
  }

  return text;
}
}

inline WiFiCredentials getWiFiCredentials()
{
  WiFiCredentials credentials;
  credentials.ssid = decodeText(WIFI_SSID, sizeof(WIFI_SSID));
  credentials.password = decodeText(WIFI_PASS, sizeof(WIFI_PASS));
  return credentials;
}
