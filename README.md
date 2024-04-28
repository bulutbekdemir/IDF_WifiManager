| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |
| **Tested On Targets** |  &#10060;|&#10060;|&#9989;|&#10060;|  &#10060; |  &#10060; | &#10060;|&#10060;|



# __

This project is under construction.

## ?
I just wanted to make a better (more features and newest idf verison support) and stable wifi manager with MQTT support. 
If you want to implement new features crete a pull request or special needs write me: [Sincap](https://github.com/bulutbekdemir).

## Project Struct 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c 
├── components
|		├── WifiManager																	---> Our WifiManager
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
|		|   |   |   └── jquery-3.1.1.min.js							---> Used libary for js
|		|   |   └── icons
|		|   |   |   └── favicon.ico
|		|   |   |   └── wifi_full.ico
|		|   |   |   └── wifi_three.ico
|		|   |   |   └── wifi_two.ico
|		|   |   |   └── wifi_err.ico
|		|   |   |   └── Original												---> Original files of .ico files	
|		|   |   |   |   └── wifi_full.png
|		|   |   |   |   └── wifi_three.png
|		|   |   |   |   └── wifi_two.png
|		|   |   |   |   └── wifi_err.png
|		|   |   |   |   └── wifi_full.svg
|		├── tMQTT 																			---> Reserved for future project.
└── README.md                  
```

# TODOs

