# Introduction

This is an Arduino Sketch for ESP8266 to report temperature/humidity from a SHT30 sensor to an MQTT server

# Hardware

* Wemos mini D1 : https://www.aliexpress.com/store/product/D1-mini-Mini-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266/1331105_32529101036.html

* Wemos mini D1 SHT30 shield : https://www.aliexpress.com/store/product/SHT30-Shield-for-WeMos-D1-mini-SHT30-I2C-digital-temperature-and-humidity-sensor-module/1331105_32762136940.html

# Configuration

## Wifi

Because the sketch use WifiManager, during the first run you will be able to connect to a Wifi AP called "AutoConnectAP" to configure the ESP8266 to then connect to the Wifi of your choice.

1/ Compile and upload the code to the ESP8266 board and restart it
2/ Connect your PC/phone/tablet to the "AutoConnectAP" wifi using the password "password"
3/ Access the web configuration page at http://192.168.4.1 to connect the ESP8266 board to your personnal wifi and choose the MQTT server address:port


## MQTT server and port

During the wifi configuration phase, you will be able to provide the MQTT server address and port on the web configuration page.

The params provided during the configuration phase will be stored in a file called config.json inside the SPIFFS partition of the ESP8266.

You can also choose to manually create and upload the configuration file config.json to the SPIFFS partition using the arduino IDE plugin esp8266fs (https://github.com/esp8266/arduino-esp8266fs-plugin).

# OTA

This sketch use the arduino OTA library, so after the first sketch upload using serial, you should be able to see the device along with his ip address in the "Tools>Ports" menu of the arduino IDE.

If the device is correctly detected, you will be able to directly send updates using this new "port" without the need of a wired (serial) connection.

For this to work, your computer need to be connected to the same wifi network as the esp8266.

# Libraries used

* https://github.com/tzapu/WiFiManager
* https://github.com/bblanchon/ArduinoJson
* https://github.com/wemos/WEMOS_SHT3x_Arduino_Library
* https://github.com/256dpi/arduino-mqtt by Joël Gähwiler

# External links

* Arduino Core : https://github.com/esp8266/Arduino
* Wemos CC store on Aliexpress : https://wemoscc.aliexpress.com
* MQTT Site : http://mqtt.org
