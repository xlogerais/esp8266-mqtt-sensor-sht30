# Introduction

This is an Arduino Sketch for ESP8266 to report temp/hum from a SHT30 sensor through MQTT protocol

# Hardware

* Wemos mini D1 : https://www.aliexpress.com/store/product/D1-mini-Mini-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266/1331105_32529101036.html

* Wemos mini D1 SHT30 shield : https://www.aliexpress.com/store/product/SHT30-Shield-for-WeMos-D1-mini-SHT30-I2C-digital-temperature-and-humidity-sensor-module/1331105_32762136940.html

# Configuration

## Wifi

Because the sketch use WifiManager, during the first run you will be able to connect to a Wifi AP to configure the ESP8266 to connect to the Wifi of your choice.

## MQTT server and port

During this phase, you will be able to provide the MQTT server address and port.

The params are stored in a file called config.json in the SPIFFS partition of the ESP8266.

You can directly upload your config file to SPIFFS using the arduino IDE plugin esp8266fs (https://github.com/esp8266/arduino-esp8266fs-plugin).


# OTA

The sketch use the arduino OTA library, so after first upload using serial, you should be able to see the device along with his ip address in the "Tools>Ports" menu of the arduino IDE.

If the device is correctly detected, you will be able to directly send updates using this new "port" without a wired serial connection.

For this, your computer need to be connected to the same wifi network as the esp8266.

# Libraries used

* https://github.com/tzapu/WiFiManager
* https://github.com/bblanchon/ArduinoJson
* https://github.com/256dpi/arduino-mqtt by Joël Gähwiler

# External links

* Arduino Core : https://github.com/esp8266/Arduino

* Wemos CC store on Aliexpress : https://wemoscc.aliexpress.com
