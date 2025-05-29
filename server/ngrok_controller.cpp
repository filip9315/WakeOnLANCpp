#include "ngrok_controller.h"
#include <iostream>
#include <string>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::json;

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* resp = static_cast<std::string*>(userdata);
    resp->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

string ngrok_controller::setup() {
    string address = "";
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL.\n";
        return "";
    }

    std::string response;
    CURLcode res;

    // --- First attempt to get tunnel (maybe ngrok is already running) ---
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4040/api/tunnels");
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
         try {
            json resp_json = json::parse(response);
            if (resp_json.count("tunnels") && resp_json["tunnels"].size() > 0) {
                address = resp_json["tunnels"][0]["public_url"];
                std::cout << "Found existing ngrok tunnel: " << address << "\n";
                // Clean up and return immediately
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return address;
            }
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Failed to parse ngrok response on first try: " << e.what() << "\n";
        }
    } else {
         std::cout << "Initial CURL error or no tunnels found: " << curl_easy_strerror(res) << "\n";
    }


    // --- If no tunnel found or connection failed, attempt to start ngrok ---
    std::cout << "Attempting to start ngrok...\n";
    std::string token = NGROK_AUTHTOKEN;
    if (!token.empty()) {
        std::string cmd = std::string("ngrok config add-authtoken ") + token + " && ngrok http 8080 --log=stdout &";
        std::system(cmd.c_str());
    } else {
        std::cerr << "NGROK_AUTHTOKEN environment variable not set.\n";
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return "";
    }

    // --- Implement a wait loop for the API to become available ---
    std::cout << "Waiting for ngrok API to become available...\n";
    int retry_count = 0;
    const int max_retries = 15;
    const std::chrono::seconds retry_delay(1);

    bool api_ready = false;
    while (retry_count < max_retries) {
        std::this_thread::sleep_for(retry_delay); // Wait before retrying
        response.clear(); // Clear previous response
        std::cout << "Attempting to connect to ngrok API (Attempt " << retry_count + 1 << "/" << max_retries << ")...\n";

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            try {
                json resp_json = json::parse(response);
                if (resp_json.count("tunnels") && resp_json["tunnels"].size() > 0) {
                    address = resp_json["tunnels"][0]["public_url"];
                    std::cout << "Successfully retrieved ngrok tunnel URL: " << address << "\n";
                    api_ready = true;
                    break;
                } else {
                     std::cerr << "API responded, but no tunnels found yet.\n";
                }
            } catch (const nlohmann::json::exception& e) {
                std::cerr << "Failed to parse ngrok response during wait loop: " << e.what() << "\n";
                std::cerr << "Raw response:\n" << response << "\n";
            }
        } else {
            std::cerr << "CURL error during wait loop: " << curl_easy_strerror(res) << "\n";
        }

        retry_count++;
    }

    if (!api_ready) {
         std::cerr << "Failed to connect to ngrok API or retrieve tunnel after multiple retries.\n";
    }


    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return address;
}
