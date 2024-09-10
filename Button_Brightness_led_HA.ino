#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Praveen act"; // Enter your WiFi name
const char *password = "slact7204813084";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.0.101";
const char *topic = "Homeassistant/value";
const char *mqtt_username = "mosquitto";
const char *mqtt_password = "password_not_changed**__";
const int mqtt_port = 1883;

// PWM configuration
const int pwmFrequency = 1000; // 1 kHz
const int pwmResolution = 10;  // 10-bit resolution (0-1023)
const int ledChannel = 0;      // PWM channel for LED

WiFiClient espClient;
PubSubClient client(espClient);

const int ledpin = 16; // GPIO 16

void setup() {
    Serial.begin(115200);

    // Configure PWM
    pinMode(ledpin,OUTPUT);
    ledcAttach(ledpin,pwmFrequency,pwmResolution);

    // Connecting to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    // Connecting to MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-test-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("HA MQTT broker connected");
            client.subscribe(topic);
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}
bool isNumber(String str){
      for (int i=0; i<str.length(); i++){
        if(!isDigit(str[i])) {
          return false;
        }
      }
      return true;
    }
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    // Convert the payload (byte array) to a String
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Received message: ");
    Serial.println(message);
    // Check and process the message
    if (isNumber(message)) {
        int buttonValue = message.toInt();
        if (buttonValue >= 0 && buttonValue <= 100) {
            int dutyCycle = map(buttonValue, 0, 100, 0, 1023);
            ledcWrite(ledpin, dutyCycle);
        }
    } else if (message == "OFF") {
        ledcWrite(ledpin, 0); // Turn off
    }
    }


void loop() {
    client.loop();
}
