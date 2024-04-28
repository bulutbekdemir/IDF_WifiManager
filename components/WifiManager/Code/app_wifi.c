/*!
*	@file wifi_app.c
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 1.1.3 @link https://semver.org/ (Semantic Versioning)
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_err.h"
#include "esp_log.h"
#include "lwip/netdb.h"

#include "app_FreeRTOS.h"
#include "app_wifi.h"
#include "app_http_server.h"

//Tag for logging
static const char *TAG = "WIFI_APP";

//Used for returning the wifi configuration
wifi_config_t *wifi_config = NULL;

//Used for returning the scanned wifi networks
wifi_ap_record_t *wifi_ap_records = NULL;

//Used to track number of retries for wifi connection
static int g_wifi_connect_retries;

//Queue handle for wifi application
static QueueHandle_t wifi_app_queue_handle;

//netif object for Station mode and AP mode
esp_netif_t *esp_sta_netif;
esp_netif_t *esp_ap_netif;

//! @todo: Add function declarations.

/*!
 * @brief Wifi Application Event Handler
 * @param arg_data, that is passed to the handler when its called
 * @param event_base, the event base that the event is registered to
 * @param event_id, the event id that is being called
 * @param event_data, the data that is passed to the event
 * 
 */
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if(event_base == WIFI_EVENT)
	{
		switch(event_id)
		{
			case WIFI_EVENT_STA_START:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
				break;
			case WIFI_EVENT_STA_CONNECTED:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
				break;
			case WIFI_EVENT_STA_DISCONNECTED:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");

				wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = (wifi_event_sta_disconnected_t *)malloc(sizeof(wifi_event_sta_disconnected_t));
				*wifi_event_sta_disconnected = *((wifi_event_sta_disconnected_t *)event_data);

				if(g_wifi_connect_retries < MAX_CONNECTION_RETRIES)
				{
					esp_wifi_connect();
					g_wifi_connect_retries++;
				}
				else
				{
					wifi_app_send_message(WIFI_APP_MSG_STA_DISCONNECTED);
				}
				break;
			case WIFI_EVENT_STA_AUTHMODE_CHANGE:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_AUTHMODE_CHANGE");
				break;
			case WIFI_EVENT_STA_WPS_ER_SUCCESS:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_SUCCESS");
				break;
			case WIFI_EVENT_STA_WPS_ER_FAILED:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_FAILED");
				break;
			case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_TIMEOUT");
				break;
			case WIFI_EVENT_STA_WPS_ER_PIN:
				ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_PIN");
				break;
			case WIFI_EVENT_AP_START:
				ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
				break;
			case WIFI_EVENT_AP_STOP:
				ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
				break;
			case WIFI_EVENT_AP_STACONNECTED:
				ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
				break;
			case WIFI_EVENT_AP_STADISCONNECTED:
				ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
				break;
			case WIFI_EVENT_AP_PROBEREQRECVED:
				ESP_LOGI(TAG, "WIFI_EVENT_AP_PROBEREQRECVED");
				break;
			case WIFI_EVENT_SCAN_DONE:
				ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
				http_server_send_message(HTTP_SERVER_MSG_WIFI_SCAN_DONE);
				break;
			default:
				break;
			}
	}else if(event_base == IP_EVENT)
	{
		switch(event_id)
		{
			case IP_EVENT_STA_GOT_IP:
				ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
				wifi_app_send_message(WIFI_APP_MSG_STA_CONNECTED_GOT_IP);
				break;
			case IP_EVENT_STA_LOST_IP:
				ESP_LOGI(TAG, "IP_EVENT_STA_LOST_IP");
				break;
			case IP_EVENT_AP_STAIPASSIGNED:
				ESP_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
				break;
			case IP_EVENT_GOT_IP6:
				ESP_LOGI(TAG, "IP_EVENT_GOT_IP6");
				break;
			default:
				break;
		}
	}
}

/*!
 * @brief Event handler for wifi application
 * 
*/
static void wifi_app_event_handler_init(void)
{
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_event_handler_instance_t instance_wifi_event;
	esp_event_handler_instance_t instance_ip_event;

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_wifi_event));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_app_event_handler, NULL, &instance_ip_event));
}

/*! 
 * @brief Initializes the TCP stack and default wifi configuration
*/
static void wifi_app_default_wifi_init(void)
{
	//Initialize the TCP stack
	ESP_ERROR_CHECK(esp_netif_init());

	//Default wifi configuration
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_sta_netif = esp_netif_create_default_wifi_sta();
	esp_ap_netif = esp_netif_create_default_wifi_ap();
}

/*!
 * @brief Configures the softAP and assignes the static IP address
 * 
*/
static void wifi_app_soft_ap_config(void)
{
	//Configuration for the softAP
	wifi_config_t ap_config = {
		.ap = {
			.ssid = WIFI_AP_SSID,
			.password = WIFI_AP_PASS,
			.channel = WIFI_AP_CHANNEL,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK,
			.ssid_hidden = WIFI_AP_SSID_HIDDEN,
			.max_connection = WIFI_AP_MAX_CONNECTIONS,
			.beacon_interval = WIFI_AP_BEACON_INTERVAL,
		},
	};

	//Configuration DHCP for the IP address for SoftAP
	esp_netif_ip_info_t ap_ip_info;
	memset(&ap_ip_info, 0, sizeof(ap_ip_info)); //we want to set everything to 0 for clean start

	//Must call it first
	esp_netif_dhcps_stop(esp_ap_netif); ///> stop the DHCP server before updating DHCP related information
	
	inet_pton(AF_INET, WIFI_AP_IP_ADDR, &ap_ip_info.ip);
	inet_pton(AF_INET, WIFI_AP_IP_GATEWAY, &ap_ip_info.gw); ///> Assign the Static Ip ,gateway and netmask address'
	inet_pton(AF_INET, WIFI_AP_IP_NETMASK, &ap_ip_info.netmask);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_ap_netif, &ap_ip_info));
	ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_ap_netif)); ///> start the DHCP server after updating DHCP related information

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); ///> Set the wifi mode to Access Point and Station
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config)); ///> Set the configuration for the Access Point
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BANDWIDTH)); ///> Set the bandwidth for the Access Point, default is 20Mhz best for low change of interference
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE)); ///> Set the power save mode for the wifi, default is NONE
}

/*!
 * @brief Task for wifi application
 * 
 * @param pvParameters 
*/
BaseType_t wifi_app_send_message(wifi_app_message_e msgID)
{
	wifi_app_queue_message_t msg;
	msg.msgID = msgID;

	return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

/*!
* @brief Connects to the station
*
*/
static void wifi_app_connect_sta(void)
{
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_app_get_wifi_config()));
	ESP_ERROR_CHECK(esp_wifi_connect());	
}

static void wifi_app_close_ap(void)
{
	wifi_mode_t mode;
	ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mode(&mode));
	if(mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
	{
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	}
}

static void wifi_app_scan_wifi_networks(void)
{
	wifi_ap_records = (wifi_ap_record_t *)malloc(MAX_SCAN_LIST_SIZE * sizeof(wifi_ap_record_t));
	uint16_t number_of_ap = 0;

	wifi_scan_config_t scan_config = {
		.ssid = (uint8_t *)WIFI_SCAN_SSID,
		.bssid = (uint8_t *)WIFI_SCAN_BSSID,
		.channel = WIFI_SCAN_CHANNEL,
		.show_hidden = WIFI_SCAN_SHOW_HIDDEN,
		.scan_type = WIFI_SCAN_TYPE,
		.scan_time.active.min = WIFI_SCAN_TIME_MIN,
		.scan_time.active.max = WIFI_SCAN_TIME_MAX,
	};

	ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number_of_ap, wifi_ap_records));
	ESP_ERROR_CHECK(esp_wifi_scan_stop());

}

/*!
 * @brief Task for wifi application
 * 
 * @param pvParameters 
*/
static void wifi_app_task(void *pvParameters)
{
	wifi_app_queue_message_t msg;
	
	wifi_app_event_handler_init();

	wifi_app_default_wifi_init();

	wifi_app_soft_ap_config();

	ESP_ERROR_CHECK(esp_wifi_start());

	wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

	while(1)
	{
		if(xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY) == pdTRUE)
		{
			switch(msg.msgID)
			{
				case WIFI_APP_MSG_START_HTTP_SERVER:
					ESP_LOGI(TAG, "Starting HTTP server");
					http_server_init();
					break;
				case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
					ESP_LOGI(TAG, "Connecting from HTTP server");

					///> Attempt to connect to the sta
					wifi_app_connect_sta();

					///>Set current number of retries to 0
					g_wifi_connect_retries = 0;

					///> Let the http server know the attempt to connect to the sta
					http_server_send_message(HTTP_SERVER_MSG_WIFI_CONNECT_INIT);
					break;
				case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
					ESP_LOGI(TAG, "Connected and got IP");
					http_server_send_message(HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS);
					break;
				case WIFI_APP_MSG_STA_DISCONNECTED:
					ESP_LOGI(TAG, "Disconnected from the station");
					http_server_send_message(HTTP_SERVER_MSG_WIFI_CONNECT_FAIL);
					break;
				case WIFI_APP_MSG_SCAN_WIFI_NETWORKS:
					ESP_LOGI(TAG, "Scanning for wifi networks"); 
					wifi_app_scan_wifi_networks();
					break;
				case WIFI_APP_MSG_CLOSE_AP:
					ESP_LOGI(TAG, "Closing the AP");
					wifi_app_close_ap();
					break;
				
				default:
					break;
			}
		}
	}
}

/*!
*	@brief Gets the wifi configuration
*/
wifi_config_t* wifi_app_get_wifi_config(void)
{
	return wifi_config;
}

/*!
* @brief Gets the scan list of wifi networks
*
*/
wifi_ap_record_t* wifi_app_get_scanned_wifi_networks(void)
{
	return wifi_ap_records;
}

/*!
 * @brief Initializes the wifi application
 * 
*/
void wifi_app_init(void)
{
	ESP_LOGI(TAG, "Initializing wifi application");

	esp_log_level_set("wifi", ESP_LOG_NONE);

	//Allocate memory for the wifi configuration
	wifi_config = (wifi_config_t *)malloc(sizeof(wifi_config_t));
	memset(wifi_config, 0, sizeof(wifi_config_t));
	
	//Create the wifi application queue
	wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));
	if(wifi_app_queue_handle == NULL)
	{
		ESP_LOGE(TAG, "Failed to create wifi application queue");
		return;
	}

	xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_CORE_ID);

}