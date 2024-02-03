#ifndef SNOW_API_H
#define SNOW_API_H

#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_http_client.h>
#include "wifi_creds.h"
#include "snow_data_parser.h"

void snow_api_init();

struct snowData get_snow_data();

#endif // SNOW_API_H