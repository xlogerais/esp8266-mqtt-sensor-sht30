/***************************************************

 Basic sketch for ESP8266 with WifiManager and OTA

****************************************************/

/*********************/
/*     Pin Mapping   */
/*     for D1 mini   */
/*********************/

// Digital Pin to GPIO mapping for Wemos D1 mini :

#define D0  16  // D0  => GPIO16
#define D1   5  // D1  => GPIO05 (SCL)
#define D2   4  // D2  => GPIO04 (SDA)
#define D3   0  // D3  => GPIO00
#define D4   2  // D4  => GPIO02 (LED builtin)
#define D5  14  // D5  => GPIO14
#define D6  12  // D6  => GPIO12
#define D7  13  // D7  => GPIO13
#define D8  15  // D8  => GPIO15
#define D9   3  // D9  => GPIO03 (RX)
#define D10  1  // D10 => GPIO01 (TX)

/*********************/
/*     Libraries     */
/*********************/

// Config file
#include <FS.h>          //this needs to be first, or it all crashes and burns...
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

// WifiManager
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

// OTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// MQTT
#include <MQTTClient.h>  // https://github.com/256dpi/arduino-mqtt by Joël Gähwiler

// SHT30
#include <WEMOS_SHT3X.h>

/*********************/
/*     Parameters    */
/*********************/

#define SERIAL_SPEED    115200

// Module Name
#define MODULE_NAME     "mqttsensor"

// Default Wifi config for AP Mode
#define WLAN_SSID       "AutoConnectAP"
#define WLAN_PASS       "password"

// Default MQTT params
#define MQTT_SERVER     "mqtt"
#define MQTT_SERVERPORT "1883" // use 8883 for SSL
#define MQTT_USERNAME   "not_used"
#define MQTT_KEY        "not_used"
#define MQTT_PREFIX     "smarthome"

// SHT30 Sensor wiring
#define SHT30_ADDR   0x45

#define READ_INTERVAL 5000 // ms

/*********************/
/*     Global vars   */
/*********************/

//define your default values here, if there are different values in config.json, they are overwritten.

char module_name[40] = MODULE_NAME;
char mqtt_server[40] = MQTT_SERVER;
char mqtt_port[6] = MQTT_SERVERPORT;
//char blynk_token[34] = "YOUR_BLYNK_TOKEN";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
    shouldSaveConfig = true;
}

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient wifi;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Create a mqtt MQTTClient class
MQTTClient mqtt;

// SHT30 Sensor
SHT3X sht30(SHT30_ADDR);

// Vars for storing humidity and temperature
float humidity, temperature;
unsigned long lastread; // time of last read

/*************************** Sketch Code ************************************/

void setup() {

  // Initialize serial port
  Serial.begin(SERIAL_SPEED);

  // Display welcome message
  Serial.println("");
  Serial.println("ESP8266 with Arduino Core");
  Serial.println("with following features :");
  Serial.println("  * WifiManager");
  Serial.println("  * mDNS");
  Serial.println("  * OTA");
  Serial.println("  * MQTT");
  Serial.println("");

  // Read configuration file config.json from SPIFFS
  Serial.println("Mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("Mounted file system");

    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("Reading config file config.json");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("Opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nParsed json");

          strcpy(module_name, json["module_name"]);
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          //strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // Set hostname
  Serial.print("Hello, my name is "); Serial.println(module_name);
  WiFi.hostname(module_name);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_module_name("name", "module name", module_name, 40);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
  //WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 32);

  //add all your parameters here
  wifiManager.addParameter(&custom_module_name);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  //wifiManager.addParameter(&custom_blynk_token);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(WLAN_SSID,WLAN_PASS)) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Connected...yeey :)");

  //read updated parameters
  strcpy(module_name, custom_module_name.getValue());
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port,   custom_mqtt_port.getValue());
  //strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["module_name"] = module_name;
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    //json["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  /* Serial.print("local ip : "); */
  /* Serial.println(WiFi.localIP()); */
  /* Serial.print("mqtt_server : "); */
  /* Serial.println(mqtt_server); */
  /* Serial.print("mqtt_port : "); */
  /* Serial.println(mqtt_port); */

  // Connect to MQTT server
  mqtt_connect(mqtt_server,mqtt_port,module_name);

  // OTA
  ota_init(module_name);

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    // OTA
    ArduinoOTA.handle();

    // MQTT
    if (!mqtt.connected()) {
      mqtt_connect(mqtt_server,mqtt_port,module_name);
    }
    else
    {
      mqtt.loop();
    }

  }
  else
  {
    Serial.println("Not connected to wifi");
    //TODO : handle timeout to reset device
  }

  // SHT03 Sensor read
  if((millis() - lastread) > READ_INTERVAL) // time to update
  {

    sht30.get();
    lastread = millis();
    temperature = sht30.cTemp;
    humidity = sht30.humidity;

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\t");
    Serial.println("");

    if (mqtt.connected()) {

      char topic[50],payload[6];

      sprintf(topic,"%s/%s/%s",MQTT_PREFIX,module_name,"temperature");
      sprintf(payload,"%s",f2s(temperature,0));
      mqtt.publish(topic,payload);

      sprintf(topic,"%s/%s/%s",MQTT_PREFIX,module_name,"humidity");
      sprintf(payload,"%s",f2s(humidity,0));
      mqtt.publish(topic,payload);

    }

  }

}
