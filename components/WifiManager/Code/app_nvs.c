/*!
* @file app_nvs.c
*
*	@date 2024
* @author Bulut Bekdemir
* 
* @copyright BSD 3-Clause License
* @version 0.1.1 @link https://semver.org/ (Semantic Versioning)
*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "app_wifi.h"
#include "app_nvs.h"

// Tag for logging
static const char *TAG = "app_nvs";

// NVS namespace
static const char nvs_sta_namespace[] = "nvs_sta_creds"; ///> NVS key max length is 15 characters

/*!
* @brief Save the STA credentials to NVS 
*
*/
esp_err_t app_nvs_save_sta_creds(void)
{
	nvs_handle_t nvshandle;
	esp_err_t	esp_err;
	ESP_LOGI(TAG, "Saving creds to the nvs");

	wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

	if(wifi_sta_config)
	{
		esp_err = nvs_open(nvs_sta_namespace, NVS_READWRITE, &nvshandle);
		if(esp_err != ESP_OK)
		{
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return esp_err;
		}

		esp_err = nvs_set_blob(nvshandle, "ssid", wifi_sta_config->sta.ssid, MAX_SSID_LENGTH);
		if(esp_err != ESP_OK)
		{
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return esp_err;
		}

		esp_err = nvs_set_blob(nvshandle, "password", wifi_sta_config->sta.password, MAX_PASSWORD_LENGTH);
		if(esp_err != ESP_OK)
		{
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return esp_err;
		}	

		esp_err = nvs_commit(nvshandle);
		if(esp_err != ESP_OK)
		{
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return esp_err;
		}	

		nvs_close(nvshandle);
		return ESP_OK;
	}
	return ESP_FAIL;
}

/*!
* @brief Load the STA credentials from NVS
*
*/
bool app_nvs_load_sta_creds(void)
{
	nvs_handle_t nvshandle;
	esp_err_t esp_err;

	if(nvs_open(nvs_sta_namespace, NVS_READONLY, &nvshandle) == ESP_OK)
	{
		wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

		memset(wifi_sta_config, 0x0, sizeof(wifi_config_t));

		size_t wifi_config_size = sizeof(wifi_config_t);
		uint8_t *wifi_config_buff = (uint8_t *)malloc(sizeof(uint8_t) * wifi_config_size);
		memset(wifi_config_buff, 0x0, wifi_config_size);

		esp_err = nvs_get_blob(nvshandle, "ssid", wifi_config_buff, &wifi_config_size);
		if(esp_err != ESP_OK)
		{
			free(wifi_config_buff);
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return false;
		}
		memcpy(wifi_sta_config->sta.ssid,wifi_config_buff,wifi_config_size);

		esp_err = nvs_get_blob(nvshandle, "password", wifi_config_buff, &wifi_config_size);
		if(esp_err != ESP_OK)
		{
			free(wifi_config_buff);
			ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
			return false;
		}
		memcpy(wifi_sta_config->sta.password,wifi_config_buff,wifi_config_size);

		free(wifi_config_buff);
		nvs_close(nvshandle);
		///> Check if the SSID is valid or not and return true if it is
		return wifi_sta_config->sta.ssid[0] != '\0';
	}
	else
	{
		return false;
	}
}

/*!
* @brief Clear the STA credentials from NVS
*
*/
esp_err_t app_nvs_clear_sta_creds(void)
{
	nvs_handle_t nvshandle;
	esp_err_t esp_err;

	esp_err = nvs_open(nvs_sta_namespace, NVS_READWRITE, &nvshandle);
	if(esp_err != ESP_OK)
	{
		ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
		return esp_err;
	}

	esp_err = nvs_erase_all(nvshandle);
	if(esp_err != ESP_OK)
	{
		ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
		return esp_err;
	}

/*
	esp_err = nvs_erase_key(nvshandle, "password");
	if(esp_err != ESP_OK)
	{
		ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
		return esp_err;
	}
*/ //!TODO: Make it possible to erase any instance of the NVS

	esp_err = nvs_commit(nvshandle);
	if(esp_err != ESP_OK)
	{
		ESP_LOGE(TAG, "ERROR: %s opening NVS handle!", esp_err_to_name(esp_err));
		return esp_err;
	}

	nvs_close(nvshandle);
	
	ESP_LOGI(TAG, "Cleared the NVS");

	return ESP_OK;
}