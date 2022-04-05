#include <iostream>
#include "esp_tls.h"
#include "esp_http_client.h"
#include "ThingSpeak.h"
#include "wifi_utils.h"

#define SSID "SSID"
#define PASSWORD "PASSWORD"
#define THINGSPEAK_CHANNEL_ID "CHANNEL_ID"
#define THINGSPEAK_API_KEY "API_KEY"   // Write api key

extern "C" void app_main() 
{
    // Wifi Connection
    wifiInit(SSID, PASSWORD);




    // Customized created_at, has to be a string formatted with ISO8061
    // Ignore it if you dont want to set time manually. Thingspeak will do it for you by setting created_at = "".
    char datetime_iso8061[50];
    tm datetime;
    datetime.tm_year = 2022 - 1900;
    datetime.tm_mon = 4 - 1;    
    datetime.tm_mday = 5;
    datetime.tm_hour = 3;
    datetime.tm_min = 0;
    datetime.tm_sec = 0;
    strftime(datetime_iso8061, 50, "%FT%H:%M:%S+01:00", &datetime);   // +01:00 is timezone
    ESP_LOGI("time", "%s", datetime_iso8061);


    // Create Feed
    ThingSpeak::Feed feed;
    feed.created_at = "";   // setting it to "" uses current datetime, setting to datetime_iso8061 stores the data with this specific datetime
    feed.fields.push_back({1, 11.11f}); //< field1 = 11.11f
    feed.fields.push_back({2, 22.22f}); //< field2 = 22.22f
    feed.fields.push_back({3, 33.33f}); //< field3 = 33.33f

    // Post Feed  
    ThingSpeak client(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
    client.post(feed);
}
