#ifndef _THING_SPEAK_H_
#define _THING_SPEAK_H_

#include <iostream>
#include "esp_http_client.h"
#include "esp_log.h"


#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>
#include "json11.hpp"



#define THINGSPEAK_TAG "ThingSpeak"
#define THINGSPEAK_FORMAT_JSON 0


extern const char cert_start[] asm("_binary_cert_pem_start");
extern const char cert_end[] asm("_binary_cert_pem_end");

class ThingSpeak {
public:
    ThingSpeak(const char *channel_id, const char *api_key)
        : channel_id(channel_id), api_key(api_key) 
    {
        url_json = std::string(url_base) + "/channels/" + channel_id + "/bulk_update.json";
        url_single = std::string(url_base) + "/update?api_key=" + api_key;
    }

    struct Feed 
    {
        std::vector<std::pair<int, float>> fields;      // field, value
        std::string created_at;
        
    };

    bool post(const std::vector<Feed> &feeds) 
    {
        return postMulti(url_json, "application/json", generateJson(feeds));
    }

    bool post(const Feed &feed) 
    {
        std::string url = url_single;
        url += "&created_at=" + feed.created_at;
        for (auto field : feed.fields)
        url += "&field" + std::to_string(field.first) + "=" +
                std::to_string(field.second);
        return postSingle(url);
    }

private:

    const char *host = "api.thingspeak.com";
    const char *url_base = "https://api.thingspeak.com";

    
    const char *channel_id;
    const char *api_key;
    std::string url_json;
    std::string url_plain;
    std::string url_single;

    std::string generateJson(const std::vector<Feed> &feeds) 
    {
        json11::Json::object obj;
        obj["write_api_key"] = api_key;
        json11::Json::array updates_ary;
        for (const auto &feed : feeds) {
            json11::Json::object obj_field;
            obj_field["created_at"] = feed.created_at;
            for (const auto &field : feed.fields) 
            {
                std::stringstream field_str;
                field_str << "field" << field.first;
                obj_field[field_str.str()] = field.second;
            }
            updates_ary.push_back(json11::Json(obj_field));
        }
        obj["updates"] = updates_ary;
        return json11::Json(obj).dump();
    }

    const std::string generateJson(const Feed &feed) 
    {
        json11::Json::object obj;
        obj["api_key"] = api_key;
        obj["created_at"] = feed.created_at;
        for (auto field : feed.fields) {
        std::stringstream field_str;
        field_str << "field" << field.first;
        obj[field_str.str()] = field.second;
        }
        return json11::Json(obj).dump();
    }

    bool postSingle(const std::string &url, int timeout_ms = 2000) 
    {
        static esp_http_client_config_t config;
        config.method = HTTP_METHOD_GET;
        config.url = url.c_str();
        config.timeout_ms = timeout_ms;
        config.cert_pem = cert_start;
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Host", host);
        ESP_LOGI(THINGSPEAK_TAG, "HTTP starting...");
        esp_http_client_perform(client);
        ESP_LOGI(THINGSPEAK_TAG, "content: %s", url.c_str());
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(THINGSPEAK_TAG, "status code: %d", esp_http_client_get_status_code(client));
        esp_http_client_cleanup(client);
        return status_code / 100 == 2;
    }

    bool postMulti(const std::string &url, const std::string &content_type,const std::string &content, int timeout_ms = 2000) 
    {
        static esp_http_client_config_t config;
        config.method = HTTP_METHOD_POST;
        config.url = url.c_str();
        config.timeout_ms = timeout_ms;
        config.cert_pem = cert_start;

        esp_http_client_handle_t client = esp_http_client_init(&config);

        esp_http_client_set_header(client, "Host", host);
        esp_http_client_set_header(client, "Content-Type", content_type.c_str());
        esp_http_client_set_post_field(client, content.c_str(), content.size());

        ESP_LOGI(THINGSPEAK_TAG, "HTTP starting...");
        esp_http_client_perform(client);
        ESP_LOGI(THINGSPEAK_TAG, "content: %s", content.c_str());
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(THINGSPEAK_TAG, "status code: %d", esp_http_client_get_status_code(client));
        esp_http_client_cleanup(client);
        return status_code / 100 == 2;
    }
};

#endif