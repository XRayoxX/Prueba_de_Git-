#pragma once
#include <string>

std::string loginRequest(const std::string& username, const std::string& password);
int getCreditsRequest();
bool playRequest();
void rewardRequest(int score);
std::string getRankingRequest();

// NUEVO
std::string getToken();
bool isLogged();