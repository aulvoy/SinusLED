#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Hent alle login/passord fra passord.h (den blir ikke sendt opp til github)
#include <passord.h>

#define ANTALL_LED 300          //Totalt antall LED på stripen
#define ANTALL_LED_PROSENT 3    //Hvor mange LED per prosent
#define DATA_PIN 2

CRGB leds[ANTALL_LED];

const char* mqtt_topic = "Monolitten";

int LysProsent;                 // Prosent av stripe som skal lyse, 0-100%
int LysAntallLED;               // Antall LED som skal lyse, kalkuleres ut fra prosent

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifipassord);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageAntallLEDs;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageAntallLEDs += (char)message[i];
  }
  Serial.println();
  if (String(topic) == mqtt_topic) {
    Serial.print("LED % ");
    Serial.print(messageAntallLEDs);
    Serial.print("\n");
    LysProsent = messageAntallLEDs.toInt();
    LysAntallLED = LysProsent * ANTALL_LED_PROSENT;
    Serial.print("Antall LED ");
    Serial.print(String(LysAntallLED));
    Serial.print("\n");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_passord)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  delay(2000);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, ANTALL_LED);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;
    for(int i = 0; i < ANTALL_LED; i++) {
      if (i>LysAntallLED-1) {
        leds[i] = CRGB::Black;
      } else {
        leds[i] = CRGB::White;
      }
    }
    FastLED.show();
  }
}