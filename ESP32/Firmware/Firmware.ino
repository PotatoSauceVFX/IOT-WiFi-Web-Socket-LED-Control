#include <FastLED.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketClient.h>

bool ConnectToClient();

const char* ssid     = "Pluto";
const char* password = "10200330";
char path[] = "/notifications";
char host[] = "192.168.137.1";
int port = 8080;

WebSocketClient webSocketClient;

CRGB leds[1];
CRGB target[1];

// Use WiFiClient class to create TCP connections
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  FastLED.addLeds<WS2812B, 13, GRB>(leds, 1);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Gateway IP address: ");
  Serial.println(WiFi.gatewayIP());

  while (!ConnectToClient()) {}
}


void loop() {
  String data;

  if (client.connected()) {
    webSocketClient.getData(data);
    if (data.length() > 0) {

      StaticJsonDocument<200> doc;
      Serial.print("Received data: ");
      Serial.println(data);

      deserializeJson(doc, data);

      target[0] = CRGB(doc["R"], doc["G"], doc["B"]);
    }
  } else {
    Serial.println("Client disconnected.");
  }

  if (target[0].r != leds[0].r || target[0].g != leds[0].g || target[0].b != leds[0].b) {
    if (target[0].r != leds[0].r) {
      if (target[0].r < leds[0].r) {
        leds[0].r = leds[0].r - 1;
      }
      if (target[0].r > leds[0].r) {
        leds[0].r = leds[0].r + 1;
      }
    }

    if (target[0].g != leds[0].g) {
      if (target[0].g < leds[0].g) {
        leds[0].g = leds[0].g - 1;
      }
      if (target[0].g > leds[0].g) {
        leds[0].g = leds[0].g + 1;
      }
    }

    if (target[0].b != leds[0].b) {
      if (target[0].b < leds[0].b) {
        leds[0].b = leds[0].b - 1;
      }
      if (target[0].b > leds[0].b) {
        leds[0].b = leds[0].b + 1;
      }
    }
    FastLED.show();
    delay(10);
  }

}

bool ConnectToClient() {
  Serial.println("Connecting to server...");
  // Connect to the websocket server
  if (client.connect("192.168.137.1", port)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed. Retrying in 3 Seconds...");
    delay(3000);
    return false;
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = "192.168.137.1:8080";
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    return true;
  } else {
    Serial.println("Handshake failed. Retrying in 3 Seconds...");
    delay(3000);
    return false;
  }

}
