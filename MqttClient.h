#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

typedef void (*ValveCallback)(int, PinStatus);

class MqttClient {
  public:
    MqttClient(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet, IPAddress server);
    void begin(ValveCallback valveCallback);
    bool connected();
    bool loop();
    static void callback(char* topic, byte* payload, unsigned int length);
    void send(String topic, int value, bool retained);
    void send(String topic, float value, bool retained);
    void send(String topic, String value, bool retained);

  private:
    void _reconnect();
    static PinStatus _resolvePayload(byte* payload, unsigned int length); 

    byte mac[6] = {0xA8, 0x61, 0x0A, 0x50, 0xF1, 0xD7};
    IPAddress ip;
    IPAddress dns;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress server;
    
    EthernetClient ethClient;
    PubSubClient client;

    static ValveCallback _valveCallback;
};

MqttClient::MqttClient(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet, IPAddress server) {
  this->ip = ip;
  this->dns = dns;
  this->gateway = gateway;
  this->subnet = subnet;
  this->server = server;
}

void MqttClient::begin(ValveCallback valveCallback) {
  _valveCallback = valveCallback;
  Ethernet.begin(this->mac, this->ip, this->dns, this->gateway, this->subnet);
  this->client.setClient(this->ethClient);
  this->client.setServer(this->server, 1883);
  this->client.setCallback(this->callback);
  if(this->client.connect("Arduino OPTA")) {
    this->client.subscribe("huis/sproeiers/voorhuis");
    this->client.subscribe("huis/sproeiers/naasthuis");
    this->client.subscribe("huis/sproeiers/achterhuis");
  }

  Serial.println("test");
}

bool MqttClient::connected() {
  return this->client.connected();
}

PinStatus MqttClient::_resolvePayload(byte* payload, unsigned int length) {
  String payloadString;
  for (unsigned int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }
  Serial.println(payloadString);
  if(payloadString == "1") {
    return HIGH;
  }
  return LOW;
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {
  std::string my_cpp_string = topic;

  PinStatus RequestedPinStatus = MqttClient::_resolvePayload(payload, length);

  if (my_cpp_string == "huis/sproeiers/voorhuis") {
    // Code for the "voorhuis" case
    _valveCallback(3, RequestedPinStatus);
    Serial.println("Handling 'voorhuis' command.");
  } else if (my_cpp_string == "huis/sproeiers/naasthuis") {
    // Code for the "naasthuis" case
    _valveCallback(2, RequestedPinStatus);
    Serial.println("Handling 'naasthuis' command.");
  } else if (my_cpp_string == "huis/sproeiers/achterhuis") {
    // Code for the "achterhuis" case
    _valveCallback(1, RequestedPinStatus);
    Serial.println("Handling 'achterhuis' command.");
  } else {
    // Code for the default case
    Serial.println("Unknown command.");
  }
}

bool MqttClient::loop() {
  if(!this->client.connected()) {
      this->client.connect("Arduino OPTA");
  }
  this->client.loop();
}

ValveCallback MqttClient::_valveCallback = nullptr;

void MqttClient::send(String topic, int value, bool retained) {
  String stringValue = String(value);
  char charValue[16];
  char charTopic[topic.length() + 1];
  stringValue.toCharArray(charValue, 16);
  topic.toCharArray(charTopic, topic.length() + 1);
  bool result = this->client.publish(charTopic, charValue, retained);
}

void MqttClient::send(String topic, float value, bool retained) {
  String stringValue = String(value);
  char charValue[16];
  char charTopic[topic.length() + 1];
  stringValue.toCharArray(charValue, 16);
  topic.toCharArray(charTopic, topic.length() + 1);
  this->client.publish(charTopic, charValue, retained);
}

// void MqttClient::send(String topic, String value, bool retained) {
//   char charValue[value.length()];
//   char charTopic[topic.length()];
//   value.toCharArray(value, value.length());
//   topic.toCharArray(topic, topic.length());
//   this->client.publish(charTopic, charValue, retained);
// }
