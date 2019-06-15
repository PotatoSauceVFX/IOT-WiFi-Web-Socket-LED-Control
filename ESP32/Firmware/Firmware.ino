#include <EEPROM.h>
#include <Streaming.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketClient.h>

#define PIN_STATUS_LED 2
#define LED_DATA 13
#define LED_COUNT 300

bool ConnectToClient();

char wifiSSID[] = "Pluto";
char wifiPASSWORD[] = "10200330";

char path[] = "/notifications";
char host[] = "192.168.137.1";
char websocketHost[] = "192.168.137.1:3000";
int port = 3000;

WebSocketClient webSocketClient;

CRGB leds[LED_COUNT];
CRGB target[LED_COUNT];

bool STATUS_LED = true;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_STATUS_LED, OUTPUT);

  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, LED_COUNT);

  Serial << "\n\nConnecting to " << wifiSSID << endl;

  WiFi.disconnect();
  WiFi.enableAP(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(PIN_STATUS_LED, STATUS_LED);
    STATUS_LED = !STATUS_LED;
  }

  STATUS_LED = true;
  digitalWrite(PIN_STATUS_LED, STATUS_LED);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway IP address: ");
  Serial.println(WiFi.gatewayIP());
}

long ledFadeDelay = 10;
void loop() {
  String data;

  digitalWrite(PIN_STATUS_LED, client.connected());

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
