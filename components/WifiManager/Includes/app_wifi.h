/*!
* @file app_wifi.h
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 1.2.0 @link https://semver.org/ (Semantic Versioning)
*/
#ifndef WIFI_APP_H_
#define WIFI_APP_H_

#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"

#define WIFI_AP_SSID	CONFIG_WIFI_AP_SSID	// "ESP_WM_AP" is default
#define WIFI_AP_PASS	CONFIG_WIFI_AP_PASS	// "Esp1234!" is default
#define WIFI_AP_CHANNEL	CONFIG_WIFI_AP_CHANNEL	// 1 is default
#define WIFI_AP_MAX_CONNECTIONS	CONFIG_WIFI_AP_MAX_CONNECTIONS	// 4 is default
#define WIFI_AP_SSID_HIDDEN	CONFIG_WIFI_AP_SSID_HIDDEN	// 0 is default
#define WIFI_AP_BEACON_INTERVAL	CONFIG_WIFI_AP_BEACON_INTERVAL	// 100 is default
#define WIFI_AP_IP_ADDR	CONFIG_WIFI_AP_IP_ADDR	// "192.168.0.24" is default
#define WIFI_AP_IP_GATEWAY	CONFIG_WIFI_AP_IP_GATEWAY	// "192.168.0.24" is default
#define WIFI_AP_IP_NETMASK	CONFIG_WIFI_AP_IP_NETMASK	// "255.255.255.0" is default

#define WIFI_BANDWIDTH		CONFIG_WIFI_BANDWIDTH		// WIFI_BW_HT20 is default NOTE: best for low change of interference
#define WIFI_STA_POWER_SAVE	CONFIG_WIFI_STA_POWER_SAVE	// 0 is default
#define MAX_SSID_LENGTH		CONFIG_MAX_SSID_LENGTH		// 32 is default
#define MAX_PASSWORD_LENGTH	CONFIG_MAX_PASSWORD_LENGTH	// 64 is default
#define MAX_CONNECTION_RETRIES	CONFIG_MAX_CONNECTION_RETRIES	// 5 is default

#define MAX_SCAN_LIST_SIZE	CONFIG_MAX_SCAN_LIST_SIZE	// 10 is default
#define WIFI_SCAN_SSID		CONFIG_WIFI_SCAN_SSID		// 0 is default, otherwise specify the SSID
#define WIFI_SCAN_BSSID		CONFIG_WIFI_SCAN_BSSID		// 0 is default, otherwise specify the BSSID
#define WIFI_SCAN_CHANNEL	CONFIG_WIFI_SCAN_CHANNEL		// 0 is default, 1-13 is valid
#define WIFI_SCAN_SHOW_HIDDEN	CONFIG_WIFI_SCAN_SHOW_HIDDEN	// 0 is default NOTE: set to 1 to show hidden SSIDs
#define WIFI_SCAN_TYPE		CONFIG_WIFI_SCAN_TYPE		// WIFI_SCAN_TYPE_ACTIVE is default
#define WIFI_SCAN_TIME_MIN		CONFIG_WIFI_SCAN_TIME_MIN		// 0 is default, otherwise specify the time in ms
#define WIFI_SCAN_TIME_MAX		CONFIG_WIFI_SCAN_TIME_MAX		// 0 is default, otherwise specify the time in ms

//netif object for Station mode and AP mode
extern esp_netif_t *esp_sta_netif;
extern esp_netif_t *esp_ap_netif;

/*!
 * @brief Enum for wifi application messages
 * 
 */
typedef enum wifi_app_message
{
	WIFI_APP_MSG_START_HTTP_SERVER = 0,
	WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
	WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
	WIFI_APP_MSG_STA_DISCONNECTED, 
	WIFI_APP_MSG_SCAN_WIFI_NETWORKS, 
	WIFI_APP_MSG_CLOSE_AP,
}wifi_app_message_e;

/*!
 * @brief Queue message structure
 * 
 */
typedef struct wifi_app_queue_message	
{
	wifi_app_message_e msgID;
}wifi_app_queue_message_t;
	
/*!
 * @brief Sends a message to the queue
 * 
 * @param msgID 
 * @return pdTRUE if successful, pdFAIL if failed
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/*!
 * @brief Initializes the wifi application
 * 
 */
void wifi_app_init(void);

/*!
 * @brief Gets the wifi configuration 
 *
 * @param  void
 * @return wifi_config_t 
 */
wifi_config_t* wifi_app_get_wifi_config(void);

/*!
* @brief Gets the scanned wifi networks
* @param  void
* @return wifi_ap_record_t* 
*/
wifi_ap_record_t* wifi_app_get_scanned_wifi_networks(void);

#endif /* WIFI_APP_H_ */