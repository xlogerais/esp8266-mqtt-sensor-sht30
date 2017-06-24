void mqtt_connect(const char* server, const char* port, const char* name) {

  Serial.println();
  Serial.print("Connecting to MQTT server : "); Serial.print(server); Serial.print(":"); Serial.println(port);

  mqtt.begin(server,atoi(port),wifi);

  while (!mqtt.connect(name)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected.");

  // Announce us on the announce topic
  mqtt.publish(MQTT_PREFIX"/announce", name);

  // Subscribe to our dedicated topic
  //mqtt.subscribe(MQTT_PREFIX"/"MODULE_NAME);
  //mqtt.subscribe(MQTT_PREFIX"/"MODULE_NAME"/#");

}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {

  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();

}
