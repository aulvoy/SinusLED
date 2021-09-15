#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Hent alle login/passord fra passord.h (den blir ikke sendt opp til github)
#include <passord.h>

#define ANTALL_LED 100          //Totalt antall LED på stripen
#define DATA_PIN 21

const char* mqtt_topic = "Monolitten";

int LysProsent;                 // Prosent av stripe som skal lyse, 0-100%
int LysAntallLED;               // Antall LED som skal lyse, kalkuleres ut fra prosent

WiFiClient espClient;
PubSubClient client(espClient);

NeoPixelBus<NeoRgbwFeature, Neo800KbpsMethod> strip(ANTALL_LED, DATA_PIN);

RgbwColor red(255, 0, 0, 0);
RgbwColor green(0, 255, 0, 0);
RgbwColor blue(0, 0, 255, 0);
RgbwColor white(255, 255, 255, 0);
RgbwColor wwhite(255, 255, 255, 255);
RgbwColor black(0);

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
    LysAntallLED = LysProsent;
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
  strip.Begin();
  strip.Show();
  
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
  for(int i = 0; i < ANTALL_LED; i++) {
      if (i>LysAntallLED-1) {
        strip.SetPixelColor(i, black);
      } else {
        strip.SetPixelColor(i, wwhite);
      }
  }
    strip.Show(); 
}