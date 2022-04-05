
#include <ctime>
#include <iostream>
#include "esp_tls.h"
#include "esp_http_client.h"
#include "ThingSpeak.h"
// define your parameter
#define THINGSPEAK_CHANNEL_ID "CHANNEL_ID"
#define THINGSPEAK_API_KEY "API_KEY"

extern "C" void app_main() 
{
    // wifi connection here
  
    // create a new feed
    ThingSpeak::Feed feed;
    feed.fields.push_back({1, 11.11f}); //< field1 = 11.11f
    feed.fields.push_back({2, 22.22f}); //< field2 = 22.22f
    feed.fields.push_back({3, 33.33f}); //< field3 = 33.33f

    // post the feed    
    ThingSpeak client(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
    client.post(feed);
}
