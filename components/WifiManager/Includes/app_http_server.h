/*!
*	@file app_http_server.h
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 1.0.0 @link https://semver.org/ (Semantic Versioning)
*/
#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "freertos/FreeRTOS.h"

/*!
 * @brief Enum for Connection status of Wifi
 * @note These are uses as json messages for the HTTP Server 
 * (http_server_wifi_status_json_handler)
 *
 */
typedef enum http_server_wifi_connection_status
{
	HTTP_SERVER_WIFI_CONNECTING = 0,
	HTTP_SERVER_WIFI_CONNECT_FAIL,
	HTTP_SERVER_WIFI_CONNECTED,
}http_server_wifi_connection_status_e;

/*!
 * @brief Messages for the HTTP Server
 */
typedef enum http_server_message
{
	HTTP_SERVER_MSG_WIFI_CONNECT_INIT = 0,
	HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS,
	HTTP_SERVER_MSG_WIFI_SCAN_START, 
	HTTP_SERVER_MSG_WIFI_SCAN_DONE, 
}http_server_message_e;

/*!
 * @brief Queue message structure
 */
typedef struct http_server_queue_message
{
	http_server_message_e msgID;
}http_server_queue_message_t;

/*!
 * @brief Sends a message to the queue
 * 
 * @param msgID 
 * @return pdTRUE if successful, pdFAIL if failed
 */
BaseType_t http_server_send_message(http_server_message_e msgID);

/*!
 * @brief HTTP Server initialization function
 * 
 */
void http_server_init(void);

/*!
 * @brief HTTP Server stop
 * 
 */
void http_server_stop(void);

/*!
* @brief Timer callback for closing the server
*
*/
void http_server_AP_close_timer_callback(void *arg);

#endif /* HTTP_SERVER_H_ */
