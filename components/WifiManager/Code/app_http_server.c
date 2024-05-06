/*!
*	@file http_server.c
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 3.0.3 @link https://semver.org/ (Semantic Versioning)
*/
#include "esp_http_server.h"
#include "esp_log.h"
#include "sys/param.h"
#include "esp_timer.h"

#include "app_http_server.h"
#include "app_FreeRTOS.h"
#include "app_wifi.h"

static const char *TAG = "HTTP_SERVER";

///> Wifi Connect Status
static int g_wifi_connect_status = -1;

///>Wifi Scan Result
wifi_app_wifi_scan_t *wifi_scan;

///> HTTP Server task handle
static httpd_handle_t http_server_handle = NULL;

///> HTTP Server Monitor task
static TaskHandle_t http_server_monitor_task_handle = NULL;

///> HTTP Server Monitor task message queue handle
static QueueHandle_t http_server_monitor_queue_handle = NULL;

///> ESP32 Timer configuration
static const esp_timer_create_args_t http_server_timer_AP_close_args = {
	.callback = &http_server_AP_close_timer_callback,
	.arg = NULL,
	.dispatch_method = ESP_TIMER_TASK,
	.name = "http_server_close_AP_timer"
};
esp_timer_handle_t http_server_timer_AP_close_handle;

///> Embedded binary data for index.html, app.css, app.js, jquery-3.3.1.min.js and favicon.ico
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t scan_css_start[] asm("_binary_scan_css_start");
extern const uint8_t scan_css_end[] asm("_binary_scan_css_end");
extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");
extern const uint8_t jquery_3_3_1_min_js_start[] asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[] asm("_binary_jquery_3_3_1_min_js_end");
extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

/*!
* @brief Check the firmware update status and creates the timer if the status is pending
*
* @param arg NULL
*/
static void http_server_timer_update_reset_timer(void *arg)
{
	if(g_wifi_connect_status == HTTP_SERVER_WIFI_CONNECTED)
	{
		ESP_LOGI(TAG, "Connecting to Wifi successful, resetting the timer; AP will close in 8 seconds");

		ESP_ERROR_CHECK(esp_timer_create(&http_server_timer_AP_close_args, &http_server_timer_AP_close_handle));
		ESP_ERROR_CHECK(esp_timer_start_once(http_server_timer_AP_close_handle, 8000000)); //8 seconds =  8,000,000 useconds
	}
	else
	{
		ESP_LOGW(TAG, "Connecting to Wifi failed, not resetting the timer; AP still open");
	}
}

/*!
* @brief Get the Wifi Scan Result
*/
static void http_server_get_wifi_scan_result(void)
{	
	wifi_scan = (wifi_app_wifi_scan_t *)malloc(sizeof(wifi_app_wifi_scan_t));
	wifi_scan = wifi_app_get_scanned_wifi_networks();	
	if((*wifi_scan).ap_count > 0)
	{	
		ESP_LOGI(TAG, "Scanned Wifi Networks: %d", (*wifi_scan).ap_count);
		for(int i = 0; i < (*wifi_scan).ap_count; i++)
		{
			ESP_LOGI(TAG, "SSID: %s, RSSI: %d ,AUTH: %d", (*wifi_scan).ap_records[i].ssid, (*wifi_scan).ap_records[i].rssi, (*wifi_scan).ap_records[i].authmode);
		}
	}
	else
	{
		ESP_LOGW(TAG, "No Wifi Networks found %d", wifi_scan->ap_count);
	}

	http_server_send_message(HTTP_SERVER_MSG_WIFI_SCAN_RESPONSE_GET);
	
}	


/*!
* @brief HTTP Server Monitor Task
* @note  Use the track events of the HTTP Server
*
* @param pvParameters NULL
*/
static void http_server_monitor_task(void *pvParameters)
{
	http_server_queue_message_t msg;
	while(1)
	{
		if(xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY) == pdTRUE)
		{
			switch(msg.msgID)
			{
				case HTTP_SERVER_MSG_WIFI_CONNECT_INIT:
					ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_CONNECT_INIT");
					g_wifi_connect_status = HTTP_SERVER_WIFI_CONNECTING;
					break;
				case HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS:
					ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS");
					g_wifi_connect_status = HTTP_SERVER_WIFI_CONNECTED;
					///> Create the timer for the AP to close
					http_server_timer_update_reset_timer(NULL);							
					break;
				case HTTP_SERVER_MSG_WIFI_SCAN_DONE:
					ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_SCAN_DONE");
					http_server_get_wifi_scan_result();
					break;
				case HTTP_SERVER_MSG_WIFI_SCAN_START:
					ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_SCAN_START");
					wifi_app_send_message(WIFI_APP_MSG_SCAN_WIFI_NETWORKS);
					break;
				case HTTP_SERVER_MSG_WIFI_SCAN_RESPONSE_GET:
					ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_SCAN_RESPONSE_GET");
					g_wifi_connect_status = HTTP_SERVER_MSG_WIFI_SCAN_RESPONSE_GET;
					break;
				default:
					break;
			}
		}
	}
}

/*!
* @brief HTTP Server Index Handler
* @note Handles the index.html request
*
*/
static esp_err_t http_server_index_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}

/*!
* @brief HTTP Server App CSS Handler
* @note Handles the app.css request
*
*/
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char *)scan_css_start, scan_css_end - scan_css_start);
	return ESP_OK;
}

/*!
* @brief HTTP Server App JS Handler
* @note Handles the app.js request
*
*/
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);
	return ESP_OK;
}

/*!
* @brief HTTP Server Jquery Handler
* @note Handles the jquery-3.3.1.min.js request
*
*/
static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "application/javascript");
	httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end - jquery_3_3_1_min_js_start);
	return ESP_OK;
}

/*!
* @brief HTTP Server Favicon Handler
* @note Handles the favicon.ico request
*
*/
static esp_err_t http_server_favicon_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "image/x-icon");
	httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);
	return ESP_OK;
}

/*!
* @brief HTTP Server Wifi Connect JSON Handler
* @note Handles the Wifi Connect request
*	@param req HTTP request
* @return ESP_OK 
*/
static esp_err_t http_server_wifi_connect_json_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "Wifi Connect JSON Handler");

	size_t lenSSID = 0, lenPassword = 0;
	char *ssid = NULL, *password = NULL;

	///> Get SSID header
	lenSSID = httpd_req_get_hdr_value_len(req, "ConnectSSID") + 1;
	if(lenSSID > 1)
	{
		ssid = malloc(lenSSID);
		if(httpd_req_get_hdr_value_str(req, "ConnectSSID", ssid, lenSSID) == ESP_OK)
		{
			ESP_LOGI(TAG, "SSID: %s", ssid);
		}
	}

	///> Get Password header
	lenPassword = httpd_req_get_hdr_value_len(req, "ConnectPassword") + 1;
	if(lenPassword > 1)
	{
		password = malloc(lenPassword);
		if(httpd_req_get_hdr_value_str(req, "ConnectPassword", password, lenPassword) == ESP_OK)
		{
			ESP_LOGI(TAG, "Password: %s", password);
		}
	}

	wifi_config_t* wifi_config = wifi_app_get_wifi_config();
	memset(wifi_config, 0, sizeof(wifi_config_t));
	memcpy(wifi_config->sta.ssid, ssid, lenSSID);
	memcpy(wifi_config->sta.password, password, lenPassword);
	wifi_app_send_message(WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER);

	free(ssid);
	free(password);

	return ESP_OK;
}

/*!
* @brief HTTP Server Wifi Status JSON Handler
* @note Responses with the Wifi Connection status
*	@param req HTTP request
* @return ESP_OK
*/
static esp_err_t http_server_wifi_status_json_handler(httpd_req_t *req)
{
	char wifiJSON[100];
	sprintf(wifiJSON, "{\"status\":%d}", g_wifi_connect_status);
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, wifiJSON, strlen(wifiJSON));
	return ESP_OK;
}

/*!
* @brief HTTP Server Wifi Scan Result JSON Handler
* @note Responses with the Wifi Scan Result
*	@param req HTTP request
* @return ESP_OK
*/
static esp_err_t http_server_wifi_scan_result_json_handler(httpd_req_t *req)
{
	http_server_send_message(HTTP_SERVER_MSG_WIFI_SCAN_START);
	while(true){
		if(g_wifi_connect_status == HTTP_SERVER_MSG_WIFI_SCAN_RESPONSE_GET)
		{
			char wifiScanJSON[100];
			sprintf(wifiScanJSON, "{\"status\":%d, \"ap_count\":%d}", g_wifi_connect_status, wifi_scan->ap_count);
			httpd_resp_set_type(req, "application/json");
			httpd_resp_send(req, wifiScanJSON, strlen(wifiScanJSON));
			break;
		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	return ESP_OK;
}

/*!
* @brief HTTP Server Wifi Scan Result List JSON Handler
* @note Responses with the Wifi Scan Result List
*	@param req HTTP request
* @return ESP_OK
*/
static esp_err_t http_server_wifi_scan_result_list_json_handler(httpd_req_t *req)
{
	char wifiScanJSON[1000];
	sprintf(wifiScanJSON, "{\"status\":%d, \"ap_count\":%d, \"ap_records\":[", g_wifi_connect_status, wifi_scan->ap_count);
	for(int i = 0; i < wifi_scan->ap_count; i++)
	{
		char temp[100];
		sprintf(temp, "{\"ssid\":\"%s\", \"rssi\":%d, \"authmode\":%d}", wifi_scan->ap_records[i].ssid, wifi_scan->ap_records[i].rssi, wifi_scan->ap_records[i].authmode);
		strcat(wifiScanJSON, temp);
		if(i < wifi_scan->ap_count - 1)
		{
			strcat(wifiScanJSON, ",");
		}
	}
	strcat(wifiScanJSON, "]}");
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, wifiScanJSON, strlen(wifiScanJSON));
	
	return ESP_OK;
}

/*!
 * @brief HTTP Server Configuration
 * @note Sets up the default HTTP Server configuration
 * 
 * @return If successful http server instance, otherwise NULL 
 */
static httpd_handle_t http_server_configure(void)
{
	//Generate default configuration
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	///> Create the HTTP Server Monitor Task
	xTaskCreatePinnedToCore(&http_server_monitor_task, "http_server_monitor_task", HTTP_SERVER_MONITOR_TASK_STACK_SIZE, NULL, \
												HTTP_SERVER_MONITOR_TASK_PRIORITY, &http_server_monitor_task_handle, HTTP_SERVER_MONITOR_CORE_ID);
	///> Create the HTTP Server Monitor Queue 
	http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

	config.core_id = HTTP_SERVER_CORE_ID;
	config.task_priority = HTTP_SERVER_TASK_PRIORITY;
	config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

	config.max_uri_handlers = 20;
	config.recv_wait_timeout = 10; //10 seconds
	config.send_wait_timeout = 10; //10 seconds

	ESP_LOGI(TAG, "Starting HTTP Server on port: '%d' with task priority: '%d'", config.server_port, config.task_priority);

	///> Start the httpd server
	if(httpd_start(&http_server_handle, &config) == ESP_OK)
	{
		///> Register URI handlers
		httpd_uri_t index_uri = {
			.uri = "/",
			.method = HTTP_GET,
			.handler = http_server_index_handler,
			.user_ctx = NULL
		};

		httpd_uri_t app_css_uri = {
			.uri = "/scan.css",
			.method = HTTP_GET,
			.handler = http_server_app_css_handler,
			.user_ctx = NULL
		};

		httpd_uri_t app_js_uri = {
			.uri = "/app.js",
			.method = HTTP_GET,
			.handler = http_server_app_js_handler,
			.user_ctx = NULL
		};

		httpd_uri_t jquery_uri = {
			.uri = "/jquery-3.3.1.min.js",
			.method = HTTP_GET,
			.handler = http_server_jquery_handler,
			.user_ctx = NULL
		};

		httpd_uri_t favicon_uri = {
			.uri = "/favicon.ico",
			.method = HTTP_GET,
			.handler = http_server_favicon_handler,
			.user_ctx = NULL
		};

		///> Wifi Connect JSON handler
		httpd_uri_t wifi_connect_json = {
			.uri = "/connectWifi.json",
			.method = HTTP_POST,
			.handler = http_server_wifi_connect_json_handler,
			.user_ctx = NULL
		};

		///> Wifi Connect Status JSON handler
		httpd_uri_t wifi_connect_status_json = {
			.uri = "/wifiConnectStatus",
			.method = HTTP_POST,
			.handler = http_server_wifi_status_json_handler,
			.user_ctx = NULL
		};

		///>Wifi Scan Result JSON handlerwifiScanResult
		httpd_uri_t wifi_scan_result_json = {
			.uri = "/scannedWifiNetworks",
			.method = HTTP_POST,
			.handler = http_server_wifi_scan_result_json_handler,
			.user_ctx = NULL
		};

		///>Wifi Scan Result List JSON handler
		httpd_uri_t wifi_scan_result_list_json = {
			.uri = "/listofScannedWifiNetworks",
			.method = HTTP_POST,
			.handler = http_server_wifi_scan_result_list_json_handler,
			.user_ctx = NULL
		};

		///> Register the URI handlers
		httpd_register_uri_handler(http_server_handle, &index_uri);
		httpd_register_uri_handler(http_server_handle, &app_css_uri);
		httpd_register_uri_handler(http_server_handle, &app_js_uri);
		httpd_register_uri_handler(http_server_handle, &jquery_uri);
		httpd_register_uri_handler(http_server_handle, &favicon_uri);
		///> Register the URI handlers for Wifi Connect
		httpd_register_uri_handler(http_server_handle, &wifi_connect_json);
		httpd_register_uri_handler(http_server_handle, &wifi_connect_status_json);
		///> Register the URI handlers for Wifi Scan
		httpd_register_uri_handler(http_server_handle, &wifi_scan_result_json);
		httpd_register_uri_handler(http_server_handle, &wifi_scan_result_list_json);

		return http_server_handle;
	}
	return NULL; //Only returns if failed to start the server
}

/*!
 * @brief Initialize and Start the HTTP Server
 * 
 */
void http_server_init(void)	
{
	if(http_server_handle == NULL)
	{
		http_server_handle = http_server_configure();
	}
}

/*!
* @brief Stops HTTP Server 
*/
void http_server_stop(void)
{
	if(http_server_handle != NULL)
	{
		httpd_stop(http_server_handle);
		http_server_handle = NULL;
	}
	if(http_server_monitor_task_handle != NULL)
	{
		vTaskDelete(http_server_monitor_task_handle);
		http_server_monitor_task_handle = NULL;
	}
}

/*!
* @brief Sends a message to the queue
*
* @param msgID
* @return pdTRUE if successful, pdFAIL if failed
*/
BaseType_t http_server_send_message(http_server_message_e msgID)
{
	http_server_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}

/*!
* @brief Timer callback function for OTA update
* @note Which callls the esp_restart() function for the update
*/
void http_server_AP_close_timer_callback(void *arg)
{
	if(g_wifi_connect_status != HTTP_SERVER_WIFI_CONNECTED)
	{
		ESP_LOGI(TAG, "AP stays open since Wifi is not connected");
	}
	else
	{
		ESP_LOGI(TAG, "AP will close now");
		///> Close the HTTP Server
		if(http_server_handle != NULL)
		{
			http_server_stop();
		}	
		///> Close the AP
		wifi_app_send_message(WIFI_APP_MSG_CLOSE_AP); ///> Send close AP message
		wifi_app_send_message(WIFI_APP_WRITE_STA_CREDENTIALS); ///> Save the STA credentials
	}
}