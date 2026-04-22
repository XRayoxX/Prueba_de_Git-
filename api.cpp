#include "api.h"
#include <curl/curl.h>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ApiClient::ApiClient(const std::string& baseUrl) : baseUrl(baseUrl) {}

std::string ApiClient::post(const std::string& endpoint, const std::string& jsonBody, const std::string& jwtToken) {
    CURL* curl = curl_easy_init();
    std::string response = "";
    if (!curl) return response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!jwtToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + jwtToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    std::string url = baseUrl + endpoint;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

std::string ApiClient::get(const std::string& endpoint, const std::string& jwtToken) {
    CURL* curl = curl_easy_init();
    std::string response = "";
    if (!curl) return response;

    struct curl_slist* headers = nullptr;
    if (!jwtToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + jwtToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    std::string url = baseUrl + endpoint;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

std::string ApiClient::extractField(const std::string& json, const std::string& field) {
    std::string key = "\"" + field + "\":";
    size_t pos = json.find(key);
    if (pos == std::string::npos) return "";
    pos += key.size();

    if (json[pos] == '"') {
        pos++;
        size_t end = json.find('"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }
    size_t end = json.find_first_of(",}", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

std::string ApiClient::login(const std::string& username, const std::string& password) {
    std::string body = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
    std::string response = post("/auth/login", body);
    if (response.find("\"success\":true") == std::string::npos) return "";
    return extractField(response, "token");
}

int ApiClient::getTokens(const std::string& jwtToken) {
    std::string response = get("/tokens/me", jwtToken);
    std::string val = extractField(response, "tokens");
    if (val.empty()) return -1;
    return std::stoi(val);
}

int ApiClient::winTokens(const std::string& jwtToken, int amount) {
    std::string body = "{\"amount\":" + std::to_string(amount) + "}";
    std::string response = post("/tokens/win", body, jwtToken);
    std::string val = extractField(response, "tokens");
    if (val.empty()) return -1;
    return std::stoi(val);
}

int ApiClient::loseTokens(const std::string& jwtToken, int amount) {
    std::string body = "{\"amount\":" + std::to_string(amount) + "}";
    std::string response = post("/tokens/lose", body, jwtToken);
    std::string val = extractField(response, "tokens");
    if (val.empty()) return -1;
    return std::stoi(val);
}