#ifndef PUNTAJE_H
#define PUNTAJE_H

#include "api.h"

extern ApiClient api;
extern std::string jwtToken;
extern int tokensJugador;

inline int ultimoBonusScore = 0;
inline int tokensAntesLogin = 0;
inline int tokensDescontados = 3;

// Restar 1 token al loguearse
inline void RestarTokenLogin() {

    int nuevosTokens = api.loseTokens(jwtToken,tokensDescontados);
    if(nuevosTokens >= 0){
        tokensJugador = nuevosTokens;
    }
}


// Dar 1 token cada 100 puntos
inline void RevisarBonusTokens(int score) {

    int bonusActual = score / 100;

    if(bonusActual > ultimoBonusScore){

        int nuevosTokens = api.winTokens(jwtToken,1);

        if(nuevosTokens >= 0){
            tokensJugador = nuevosTokens;
        }

        ultimoBonusScore = bonusActual;
    }
}


inline void ResetBonusTokens(){
    ultimoBonusScore = 0;
}

#endif