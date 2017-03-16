#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

# define useCredentialsFile


#ifdef useCredentialsFile
#include <credentials.h>
#else
mySSID = "    ";
myPASSWORD = "   ";
#endif


AsyncMqttClient mqttClient;

#define RELAY_PIN D7

#define MQTT_RELAY1_TOPIC     "LAB/LIGHT/MAIN/SWITCH"
#define MQTT_RELAY2_TOPIC     "LAB/LIGHT/BENCH"
#define MQTT_FEEDBACK1_TOPIC  "LAB/LIGHT/MAIN/FEEDBACK"
#define MQTT_LASTWILL_TOPIC   "LAB/LIGHT/lastwill"




void setRelay(String command) {
  if (command == "ON") digitalWrite(RELAY_PIN, HIGH);
  else digitalWrite(RELAY_PIN, LOW);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("** Connected to the broker **");

  mqttClient.subscribe(MQTT_RELAY1_TOPIC, 1);

  Serial.print("Subscribing : ");
  Serial.println(MQTT_RELAY1_TOPIC);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("** Disconnected from the broker **");
  Serial.println("Reconnecting to MQTT...");
  mqttClient.connect();
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.print("** Subscribe acknowledged **");
  Serial.print(" packetId: ");
  Serial.print(packetId);
  Serial.print(" qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("** Unsubscribe acknowledged **");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

  Serial.println("** Message received **");
  Serial.print("  topic: ");
  Serial.print(topic);
  Serial.print("  feedbackTopic: ");
  Serial.print(MQTT_RELAY1_TOPIC);
  Serial.print("  qos: ");
  Serial.print(properties.qos);
  Serial.print("  dup: ");
  Serial.print(properties.dup);
  Serial.print("  retain: ");
  Serial.print(properties.retain);
  Serial.print("  len: ");
  Serial.print(len);
  Serial.print("  index: ");
  Serial.print(index);
  Serial.print("  payload: ");
  Serial.println(payload);

  if (strcmp(topic, MQTT_RELAY1_TOPIC) == 0)  {
    setRelay(payload);
    mqttClient.publish(MQTT_FEEDBACK1_TOPIC, 1, false, payload);
    Serial.println("Publishing Feedback");
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("** Published");
  Serial.print("  packetId: ");
  Serial.print(packetId);
  Serial.print("\n\n");
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");

  // Relay PIN
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  WiFi.persistent(false);

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(mySSID, myPASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" OK");

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  //  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(IPAddress(192, 168, 0, 201), 1883);
  mqttClient.setKeepAlive(5).setCleanSession(false).setWill(MQTT_LASTWILL_TOPIC, 2, true, "my wife will get all my money").setCredentials("username", "password").setClientId("m");
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void loop() {
}
