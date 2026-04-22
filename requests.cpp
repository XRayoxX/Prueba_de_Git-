#include "requests.h"
#include "api.h"

static ApiClient api("https://arcadeumg.duckdns.org/api");
static std::string jwtToken = "";

// LOGIN
std::string loginRequest(const std::string& username, const std::string& password) {
    jwtToken = api.login(username, password);
    return jwtToken;
}

// TOKEN GLOBAL
std::string getToken() {
    return jwtToken;
}

bool isLogged() {
    return !jwtToken.empty();
}

// CRÉDITOS
int getCreditsRequest() {
    if (jwtToken.empty()) return -1;
    return api.getTokens(jwtToken);
}

// JUGAR
bool playRequest() {
    if (jwtToken.empty()) return false;

    int credits = api.getTokens(jwtToken);
    if (credits <= 0) return false;

    api.loseTokens(jwtToken, 1);
    return true;
}

// RECOMPENSA
void rewardRequest(int score) {
    if (jwtToken.empty()) return;

    int reward = score / 100;
    if (reward > 0) api.winTokens(jwtToken, reward);
}

// RANKING
std::string getRankingRequest() {
    return api.get("/ranking", "");
}