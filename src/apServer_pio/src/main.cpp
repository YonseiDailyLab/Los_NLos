#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Access Point
const char* ap_ssid = "ESP32-AP";
const char* ap_password = "12345678";

// Station
const char* sta_ssid = "Yonsei-IoT-2G";
const char* sta_password = "yonseiiot209";

// MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "yonsei_iot/esp32_test";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup_ap(){
    Serial.println();
    Serial.println("Configuring access point...");

    if (!WiFi.softAP(ap_ssid, ap_password)) {
        log_e("Soft AP creation failed.");
        while (1);
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    delay(10);
}

void setup_sta(){
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(sta_ssid);
    WiFi.begin(sta_ssid, sta_password);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}

void setup(){
    Serial.begin(115200);

    WiFi.mode(WIFI_AP_STA);
    setup_ap();
    setup_sta();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop(){
    if (!client.connected()) {
    reconnect();
    }
    client.loop();

    // 예제: 10초마다 메시지 발행
    static unsigned long lastMsg = 0;
    unsigned long now = millis();
    if (now - lastMsg > 10000) {
        lastMsg = now;
        String msg = "Hello from ESP32";
        Serial.print("Publishing message: ");
        Serial.println(msg);
        client.publish(mqtt_topic, msg.c_str());
  }
}

void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for(int i = 0; i < length; i++){
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
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