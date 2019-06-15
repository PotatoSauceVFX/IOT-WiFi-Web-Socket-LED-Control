#include <EEPROM.h>
#include <Streaming.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketClient.h>

#define LED_DATA 13
#define LED_COUNT 300

bool ConnectToClient();

char path[] = "/notifications";
char host[] = "192.168.137.1";
char websocketHost[] = "192.168.137.1:3000";
int port = 3000;

struct config_t
{
  char ssid[64];
  char password[64];
  byte R;
  byte G;
  byte B;
  bool rst;
} config;
#include "EEPROM.h"


WebSocketClient webSocketClient;

CRGB leds[LED_COUNT];
CRGB target[LED_COUNT];
bool netLED = true;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);

  EEPROM_initialize(256);
  EEPROM_readAnything(0, config);

  if (!config.rst) {
    strcpy(config.ssid, "Pluto");
    strcpy(config.password, "10200330");
    config.R = 0;
    config.G = 0;
    config.B = 0;
    config.rst = true;

    EEPROM_writeAnything(0, config);
    Serial << "Resetting EEPROM to Defaults" << endl;
  }

  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, LED_COUNT);
  leds[0].r = config.R;
  leds[0].g = config.G;
  leds[0].b = config.B;
  target[0].r = config.R;
  target[0].g = config.G;
  target[0].b = config.B;
  FastLED.show();

  Serial << "Setting LED to last saved config: " << config.R << " " << config.G << " " << config.B << endl;

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(config.ssid);

  WiFi.disconnect();
  WiFi.enableAP(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(2, netLED);
    netLED = !netLED;
  }

  netLED = true;
  digitalWrite(2, netLED);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Gateway IP address: ");
  Serial.println(WiFi.gatewayIP());
}

long ledFadeDelay = 10;
void loop() {
  String data;

  digitalWrite(2, client.connected());

  if (client.connected()) {
    webSocketClient.getData(data);
    if (data.length() > 0) {

      StaticJsonDocument<200> doc;
      Serial.print("Received data: ");
      Serial.println(data);

      deserializeJson(doc, data);

      int index = doc["I"];
      target[index] = CRGB(doc["R"], doc["G"], doc["B"]);
      ledFadeDelay = doc["T"];
    }
  } else {
    Serial.println("Client disconnected.");
    while (!ConnectToClient()) {}
  }

  for (int i = 0; i < LED_COUNT; i++) {
    if (target[i].r != leds[i].r || target[i].g != leds[i].g || target[i].b != leds[i].b) {
      if (ledFadeDelay == 0)
      {
        leds[i] = target[i];
        FastLED.show();
      }
      else
      {
        if (target[i].r != leds[i].r) {
          if (target[i].r < leds[i].r) {
            leds[i].r = leds[i].r - 1;
          }
          if (target[i].r > leds[i].r) {
            leds[i].r = leds[i].r + 1;
          }
        }

        if (target[i].g != leds[i].g) {
          if (target[i].g < leds[i].g) {
            leds[i].g = leds[i].g - 1;
          }
          if (target[i].g > leds[i].g) {
            leds[i].g = leds[i].g + 1;
          }
        }

        if (target[i].b != leds[i].b) {
          if (target[i].b < leds[i].b) {
            leds[i].b = leds[i].b - 1;
          }
          if (target[i].b > leds[i].b) {
            leds[i].b = leds[i].b + 1;
          }
        }
        FastLED.show();
        delay(ledFadeDelay);
      }
    }
  }
}

bool ConnectToClient() {
  Serial.println("Connecting to server...");
  // Connect to the websocket server
  if (client.connect(host, port)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed. Retrying in 3 Seconds...");
    delay(3000);
    return false;
  }

  // Handshake with the server

  webSocketClient.path = path;
  webSocketClient.host = websocketHost;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    return true;
  } else {
    Serial.println("Handshake failed. Retrying in 3 Seconds...");
    delay(3000);
    return false;
  }

}
