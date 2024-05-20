# THIS PROJECT IS MOVED. 
Project new repo is: 


| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |
| **Tested On Targets** |  &#10060;|&#10060;|&#9989;|&#10060;|  &#10060; |  &#10060; | &#10060;|&#10060;|

**This code is theoretically implementable for all the above devices.

# About

This project is currently under development.

## Purpose
The purpose of this project is to create a more feature-rich and stable WiFi manager with MQTT support (currently under development). 

## Contribution
If you would like to contribute to this project by implementing new features, please create a pull request. If you have any special needs or requests, feel free to contact me: [Bulut](https://github.com/bulutbekdemir).

## Project Structure 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c 
├── components
|		├── WifiManager	---> WifiManager Component
|		│   ├── CMakeLists.txt
|		│   ├── KConfig
|		|   └── Includes
|		|   |   └── app_wifi.h
|		|   |   └── app_http_server.h
|		|   |   └── app_nvs.h
|		|   |   └── app_FreeRTOS.h
|		|   └── Code
|		|   |   └── app_http_server.c
|		|   |   └── app_wifi.c
|		|   |   └── app_nvs.c
|		|   |   └── app_FreeRTOS.c
|		|   └── Web
|		|   |   └── index.html													
|		|   |   └── app.js
|		|   |   └── app.css
|		|   |   └── lib
|		|   |   |   └── jquery-3.1.1.min.js	---> Used libary for js
|		|   |   └── icons
|		|   |   |   └── favicon.ico
|		|   |   |   └── wifi_full.ico
|		|   |   |   └── wifi_three.ico
|		|   |   |   └── wifi_two.ico
|		|   |   |   └── wifi_err.ico
|		|   |   |   └── Original					 ---> Original files of .ico files	
|		|   |   |   |   └── wifi_full.png
|		|   |   |   |   └── wifi_three.png
|		|   |   |   |   └── wifi_two.png
|		|   |   |   |   └── wifi_err.png
|		|   |   |   |   └── wifi_full.svg
|		├── tMQTT 	---> Reserved for future project.
└── README.md                  
```

# TODOs

1. Scan Feature
2. Better html
3. Wifi without password
