/*
 *  Sending GPIO data to Azure Event Hub
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "config.h"

// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;

const int httpsPort = 443;

// Set Endpoint to Azure Event Hub
String url = String("/") + String(eventHubName) + String("/messages");
String contentType = "application/atom+xml;type=entry;charset=utf-8";

void sendData(String payload) {
    String data = String("POST ") + url + " HTTP/1.1\r\n" +
              "Host: " + String(host) + "\r\n" +
              "Authorization: " + String(sas) + "\r\n" +
              "Content-Type: " + String(contentType) + "\r\n" + 
              "Content-Length: " + payload.length() + "\r\n" +
              "\r\n" +
              payload + "\r\n";
  client.print(data);

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
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

  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
}

int value = 0;

void loop() {
  int sensorValue = analogRead(A0);
  String payload = "{lightResistance:" + String(sensorValue) + "}";
  sendData(payload);
  Serial.println(payload);
  delay(1000);
}
