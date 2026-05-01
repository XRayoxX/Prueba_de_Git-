#ifndef PRUEBA2_GAME_OVER_H
#define PRUEBA2_GAME_OVER_H

#include "raylib.h"
#include "Puntaje.h"
#include <algorithm>

class GameOverScreen {

private:

    Texture2D fondo;
    int startTime;
    int continueCost;

public:

    GameOverScreen(){

        fondo=LoadTexture("Imagenes/GamerOver.png");

        startTime=0;
        // FIX: el costo inicial se ancla al precio del juego (tokensDescontados)
        // así si cambias el precio en Puntaje.h, se refleja automáticamente aquí
        continueCost=tokensDescontados;
    }


    ~GameOverScreen(){

        UnloadTexture(fondo);
    }


    void Start(){

        startTime=(int)GetTime();
    }


    bool TiempoTerminado(){

        int elapsed=(int)(GetTime()-startTime);

        return elapsed>=10;
    }


    int TiempoRestante(){

        int elapsed=(int)(GetTime()-startTime);

        int restante=10-elapsed;

        // BUG FIX: nunca mostrar número negativo si el tiempo ya pasó
        return restante<0 ? 0 : restante;
    }


    bool HandleContinue(){

        if(tokensJugador<continueCost)
            return false;

        int nuevosTokens=
            api.loseTokens(
                jwtToken,
                continueCost
            );

        // BUG FIX: si la API falla (devuelve -1) no aplicamos el costo
        // ni dejamos continuar, en lugar de dejar tokens en estado inválido
        if(nuevosTokens<0)
            return false;

        tokensJugador=nuevosTokens;

        continueCost*=2;

        if(continueCost>30)
            continueCost=30;

        return true;
    }


    void Reset(){

        // FIX: al resetear también volvemos al precio base del juego
        continueCost=tokensDescontados;
        startTime=(int)GetTime();
    }


    void Draw(){

        DrawTexturePro(
            fondo,
            {
             0,0,
             (float)fondo.width,
             (float)fondo.height
            },
            {
             0,0,
             400,600
            },
            {0,0},
            0,
            WHITE
        );

        DrawText(
           TextFormat("%d",TiempoRestante()),
           185,
           280,
           50,
           YELLOW
        );

        DrawText(
          TextFormat(
          "CONTINUAR %d TOKENS",
          continueCost
          ),
          70,
          340,
          22,
          WHITE
        );

        // BUG FIX: mostrar aviso cuando no tiene tokens suficientes
        if(tokensJugador < continueCost){
            DrawText(
                "TOKENS INSUFICIENTES",
                75,
                370,
                16,
                RED
            );
        }

        Vector2 mouse=GetMousePosition();

        Rectangle btnContinue={
            100,390,200,50
        };

        Rectangle btnSalir={
            100,460,200,50
        };

        Color c1=
        CheckCollisionPointRec(
            mouse,
            btnContinue
        ) ? LIME : GREEN;

        Color c2=
        CheckCollisionPointRec(
            mouse,
            btnSalir
        ) ? MAROON : RED;

        DrawRectangleRec(
           btnContinue,
           Fade(c1,0.9f)
        );

        DrawText(
           "CONTINUAR",
           135,
           405,
           22,
           WHITE
        );

        DrawRectangleRec(
           btnSalir,
           Fade(c2,0.9f)
        );

        DrawText(
           "SALIR",
           165,
           475,
           22,
           WHITE
        );
    }


    bool ClickContinue(){

        if(!IsMouseButtonPressed(
             MOUSE_LEFT_BUTTON
           ))
            return false;

        Vector2 mouse=
            GetMousePosition();

        Rectangle btnContinue={
            100,390,200,50
        };

        if(CheckCollisionPointRec(
           mouse,
           btnContinue
           ))
           return true;

        return false;
    }


    bool ClickSalir(){

        if(!IsMouseButtonPressed(
             MOUSE_LEFT_BUTTON
           ))
            return false;

        Vector2 mouse=
            GetMousePosition();

        Rectangle btnSalir={
            100,460,200,50
        };

        if(CheckCollisionPointRec(
            mouse,
            btnSalir
            ))
            return true;

        return false;
    }
};

#endif