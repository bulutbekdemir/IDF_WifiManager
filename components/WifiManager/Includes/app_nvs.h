/*!
* @file app_wifi.h
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 0.1.0 @link https://semver.org/ (Semantic Versioning)
*/
#ifndef APP_NVS_H_
#define APP_NVS_H_

#include "esp_err.h"

/*!
* @brief Save the STA credentials to NVS 
*
*/
esp_err_t app_nvs_save_sta_creds(void);

/*!
* @brief Load the STA credentials from NVS
*
*/
bool app_nvs_load_sta_creds(void);

/*!
* @brief Clear the STA credentials from NVS
*
*/
esp_err_t app_nvs_clear_sta_creds(void);

#endif /* APP_NVS_H_ */