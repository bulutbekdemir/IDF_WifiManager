idf_component_register(SRC_DIRS Code
                    INCLUDE_DIRS Includes
                    REQUIRES nvs_flash esp_http_server esp_wifi esp_netif esp_event esp_eth lwip esp_timer
										EMBED_FILES Web/index.html Web/app.css Web/app.js Web/lib/jquery-3.3.1.min.js  #!TODO: Change to minified version of EMBED_FILES 
																Web/icons/favicon.ico Web/icons/wifi_full.ico Web/icons/wifi_three.ico 
																Web/icons/wifi_two.ico Web/icons/wifi_err.ico)