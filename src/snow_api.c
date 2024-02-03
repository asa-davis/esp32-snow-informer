#define JSMN_HEADER
#include "snow_api.h"

static char responseBody[500];
static esp_http_client_handle_t client;
static char first16ResponseChars[16] = "{\"topSnowDepth\":";

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("snow_api: WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("snow_api: WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("snow_api: WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("snow_api: WiFi got IP ... \n\n");
        break;
    default:
        printf("\n\nsnow_api: UNKNOWN WIFI EVENT: %ld\n\n", event_id);
    }
}

static void connect_to_wifi()
{
    // todo: figure out how this part works
    esp_netif_init();                   
    esp_event_loop_create_default();    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS
        }
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_connect();
}

static esp_err_t client_event_get_response_body_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
            strncpy(responseBody, (char *)evt->data, evt->data_len);
            responseBody[evt->data_len] = '\0';
            printf("snow_api: received - \n\n%s\n\n", responseBody);

            // for some reason, once in a while the first few characters of the response are missing from the event data 
            // fortunately, we know the first 16 characters are always '{"topSnowDepth":'
            if (responseBody[0] != '{')
            {
                printf("\n\nsnow_api: INCOMPLETE RESPONSE FROM API - Attempting to repair...\n");

                // try to find how many characters we're missing
                uint8_t numMissingChars = NULL;
                for (uint8_t i = 0; i < 16; i++) {
                    if (i == 15 && responseBody[0] == first16ResponseChars[i]) {
                        numMissingChars = i;
                        break;
                    }
                    if (i < 15 && responseBody[0] == first16ResponseChars[i] && responseBody[1] == first16ResponseChars[i + 1]) {
                        numMissingChars = i;
                        break;
                    }
                    if (i == 15) {
                        printf("\n\nsnow_api: UNABLE TO REPAIR INCOMPLETE RESPONSE - Parser will probably fail :(\n");
                        return ESP_FAIL;
                    }
                }

                // copy the missing characters into a new buffer
                char repairedResponse[500];
                strncpy(repairedResponse, first16ResponseChars, numMissingChars);
                repairedResponse[numMissingChars] = '\0';

                // add the rest of the response, and copy the repaired response back into the buffer
                strcat(repairedResponse, responseBody);
                strcpy(responseBody, repairedResponse);

                printf("snow_api: repaired response - \n\n%s\n\n", responseBody);
            }
            break;

        default:
            break;
    }
    return ESP_OK;
}

static void get_api_response()
{
    esp_http_client_set_header(client, "X-RapidAPI-Key", "RAPID API KEY GOES HERE");
    esp_http_client_set_header(client, "X-RapidAPI-Host", "ski-resort-forecast.p.rapidapi.com");
    esp_http_client_perform(client);
}

void snow_api_init() 
{
    connect_to_wifi();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    printf("snow_api: WiFi was initiated ...........\n\n");

    esp_http_client_config_t config_get = {
        .url = "https://ski-resort-forecast.p.rapidapi.com/Winter%20Park/snowConditions?units=i",
        .method = HTTP_METHOD_GET,
        .event_handler = client_event_get_response_body_handler
    };
    client = esp_http_client_init(&config_get);
    printf("snow_api: http client was initiated ...........\n\n");
}

struct snowData get_snow_data() 
{
    printf("snow_api: GETTING DATA FROM API!\n");
    get_api_response();
    return parse_depth_and_fresh_from_JSON(responseBody);
}
