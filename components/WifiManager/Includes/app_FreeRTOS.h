/*!
* @file app_FreeRTOS.h
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 0.1.0 @link https://semver.org/ (Semantic Versioning)
*/
#ifndef APP_FREERTOS_H_
#define APP_FREERTOS_H_

//Wifi application task
#define WIFI_APP_TASK_PRIORITY			CONFIG_WIFI_APP_TASK_PRIORITY // 5 is default	
#define WIFI_APP_TASK_STACK_SIZE		CONFIG_WIFI_APP_TASK_STACK_SIZE // 4096 is default
#define WIFI_APP_CORE_ID						CONFIG_WIFI_APP_CORE_ID //0 is default

//HTTP Server task
#define HTTP_SERVER_TASK_PRIORITY			CONFIG_HTTP_SERVER_TASK_PRIORITY // 4 is default
#define HTTP_SERVER_TASK_STACK_SIZE		CONFIG_HTTP_SERVER_TASK_STACK_SIZE // 8192 is default
#define HTTP_SERVER_CORE_ID						CONFIG_HTTP_SERVER_CORE_ID // 0 is default

//HTTP Server Monitor task
#define HTTP_SERVER_MONITOR_TASK_PRIORITY			CONFIG_HTTP_SERVER_MONITOR_TASK_PRIORITY // 3 is default
#define HTTP_SERVER_MONITOR_TASK_STACK_SIZE		CONFIG_HTTP_SERVER_MONITOR_TASK_STACK_SIZE // 4096 is default
#define HTTP_SERVER_MONITOR_CORE_ID						CONFIG_HTTP_SERVER_MONITOR_CORE_ID // 0 is default

#endif /* APP_FREERTOS_H_ */