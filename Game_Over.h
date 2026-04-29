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
        continueCost=5;
    }


    ~GameOverScreen(){

        UnloadTexture(fondo);
    }


    void Start(){

        startTime=GetTime();
    }


    bool TiempoTerminado(){

        int elapsed=(int)(GetTime()-startTime);

        return elapsed>=10;
    }


    int TiempoRestante(){

        int elapsed=(int)(GetTime()-startTime);

        return 10-elapsed;
    }


    bool HandleContinue(){

        if(tokensJugador<continueCost)
            return false;


        int nuevosTokens=
            api.loseTokens(
                jwtToken,
                continueCost
            );

        if(nuevosTokens<0)
            return false;


        tokensJugador=nuevosTokens;


        continueCost*=2;

        if(continueCost>30)
            continueCost=30;


        return true;
    }


    void Reset(){

        continueCost=5;
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