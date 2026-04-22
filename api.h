#pragma once
#include <string>

class ApiClient {
public:
    ApiClient(const std::string& baseUrl);
    std::string login(const std::string& username, const std::string& password);
    int getTokens(const std::string& jwtToken);
    int winTokens(const std::string& jwtToken, int amount);
    int loseTokens(const std::string& jwtToken, int amount);

    // ← mover estas aquí (public)
    std::string post(const std::string& endpoint, const std::string& jsonBody, const std::string& jwtToken = "");
    std::string get(const std::string& endpoint, const std::string& jwtToken = "");

private:
    std::string baseUrl;
    std::string extractField(const std::string& json, const std::string& field);
};