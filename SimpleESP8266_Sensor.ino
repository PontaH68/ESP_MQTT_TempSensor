
//  https://www.randomdiyprojects.it/simple-esp8266-mqtt-sensor-with-home-assistant/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>


//****************** CONFIGURATION ******************
#define wifi_ssid "your_router_id"
#define wifi_password "your_router_psw"

#define mqtt_server "your_mosquitto_ip" // your HA IP, example 192.XX.XXX.XX
#define mqtt_user "your_mosquitto_user"
#define mqtt_password "your_mosquitto_psw"


#define temperature_topic "sensor/temperature"
//****************** CONFIGURATION ******************

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
float temp = 0.0;

int sensorPin = 0;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting Wi-Fi...");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected:");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    // Attempt to connect

    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) { // publish frequency
     lastMsg = now;

      int reading = analogRead(sensorPin); 
      float voltage = reading * 3.3;  //3.3 if power at 3.3V
      voltage /= 1024.0; 

      float Temp = (voltage - 0.5) * 100; //conversion from voltage to temperature, the resolution of the sensor is 10 mV per degree, plus offset of 500 mV


      Serial.print("Temperature:");
      Serial.println(String(Temp).c_str());
      client.publish(temperature_topic, String(Temp).c_str(), true);
  }
}