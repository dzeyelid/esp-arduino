/**
   This source code refer to the recipe of IBM developerWorks, https://developer.ibm.com/recipes/tutorials/connect-an-esp8266-with-the-arduino-sdk-to-the-ibm-iot-foundation/ .
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//-------- Customise these values -----------
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-SSID-PASSWORD";

#define ORG "YOUR-ORG"
#define DEVICE_TYPE "YOUR-DEVICE-TYPE"
#define DEVICE_ID "YOUR-DEVICE-ID"
#define TOKEN "YOUR-DEVICE-TOKEN"
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

int inputPinButton = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int getButtonStatus() {
  return digitalRead(inputPinButton);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("callback invoked");
}

void setup() {
  // Setup serial
  Serial.begin(115200);
  Serial.println();

  // Setup Wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT client
  client.setServer(server, 1883);
  client.setCallback(callback);

  // Setup digital input
  pinMode(inputPinButton, INPUT);
}

int counter = 0;

void loop() {

  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
  }

  // Create payload
  String payload = "{\"d\":{\"myName\":\"";
  payload += DEVICE_TYPE;
  payload += ".";
  payload += DEVICE_ID;
  payload += "\",\"counter\":\"";
  payload += counter;
  payload += "\",\"buttonStatus\":\"";
  payload += getButtonStatus();
  payload += "\"}}";

  Serial.print("Sending payload: ");
  Serial.println(payload);

  if (client.publish(topic, (char*) payload.c_str())) {
    Serial.println("Publish ok");
  } else {
    Serial.println("Publish failed");
  }

  if (20 < counter) {
    counter = 0;
  }
  else {
    ++counter;
  }
  delay(2000);
}

