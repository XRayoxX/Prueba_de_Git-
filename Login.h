#ifndef PRUEBA2_LOGIN_H
#define PRUEBA2_LOGIN_H

#include "raylib.h"
#include "raygui.h"
#include <string.h>
#include <string>
#include "Puntaje.h"
#include "requests.h"
#include "api.h"

// Variables globales
extern int tokensJugador;
extern std::string jwtToken;

inline char usuario[32] = "";
inline char password[32] = "";

inline bool usuarioEditMode  = false;
inline bool passwordEditMode = false;

inline bool mostrarError = false;
inline const char* mensajeError = "";

// Colores
static const Color NEON_CYAN  = {0,255,255,255};
static const Color NEON_PINK  = {255,0,255,255};
static const Color RETRO_BLUE = {10,20,40,255};
static const Color GRID_COLOR = {0,100,150,100};

void DibujarFondoRetro() {

    ClearBackground(RETRO_BLUE);

    float w=GetScreenWidth();
    float h=GetScreenHeight();

    for(int i=0;i<=20;i++) {

        float y=h*0.7f+(i*i*0.5f);

        if(y<h)
            DrawLine(0,y,w,y,GRID_COLOR);

        DrawLine(
            w/2,
            h*0.6f,
            (i*w/10)-w/2,
            h,
            GRID_COLOR
        );
    }

    DrawRectangleGradientV(
        0,
        h*0.40f,
        w,
        h*0.30f,
        Fade(BLACK,0),
        NEON_CYAN
    );
}

void DibujarScanlines() {

    for(int i=0;i<GetScreenHeight();i+=4){
        DrawLine(
            0,i,
            GetScreenWidth(),i,
            (Color){0,0,0,40}
        );
    }
}

inline bool DibujarLogin() {
    bool loginExitoso = false;
    float w = (float)GetScreenWidth();
    float h = (float)GetScreenHeight();

    DibujarFondoRetro();

    // --- HUD SUPERIOR ---
    DrawText("1P",    35, 20, 25, WHITE);
    DrawText("00",    35, 48, 25, NEON_CYAN);

    DrawText("PLAYER", w/2 - MeasureText("PLAYER", 20)/2, 20, 20, YELLOW);
    DrawText(usuario,  w/2 - MeasureText(usuario, 20)/2,  44, 20, WHITE);

    DrawText("Puntos", w - 130, 20, 20, WHITE);
    DrawText("00",     w - 95,  48, 25, NEON_PINK);

    DrawLine(0, 85, w, 85, (Color){0, 255, 255, 40});

    // --- TOKENS (centrado) ---
    DrawText("TOKENS", w/2 - MeasureText("TOKENS", 18)/2, 96, 18, NEON_CYAN);
    DrawText(TextFormat("%d", tokensJugador),
             w/2 - MeasureText(TextFormat("%d", tokensJugador), 28)/2,
             118, 28, WHITE);

    // --- PANEL Antes / Descuento / Actual ---
    float panelW = w * 0.78f;
    float panelX = (w - panelW) / 2;
    float panelY = 158;
    DrawRectangleRounded({panelX, panelY, panelW, 52}, 0.2f, 8, {255,255,255,13});

    DrawLine(panelX + panelW/3,   panelY+8, panelX + panelW/3,   panelY+44, {255,255,255,35});
    DrawLine(panelX + panelW*2/3, panelY+8, panelX + panelW*2/3, panelY+44, {255,255,255,35});

    // Labels
    DrawText("Antes",     panelX + panelW*0/3 + 14, panelY+7,  10, GRAY);
    DrawText("Descuento", panelX + panelW*1/3 + 6,  panelY+7,  10, GRAY);
    DrawText("Actual",    panelX + panelW*2/3 + 14, panelY+7,  10, GRAY);
    // Valores
    DrawText(TextFormat("%d",  tokensAntesLogin),
             panelX + panelW*0/3 + 14, panelY+24, 18, GOLD);
    DrawText(TextFormat("-%d", tokensDescontados),
             panelX + panelW*1/3 + 14, panelY+24, 18, RED);
    DrawText(TextFormat("%d",  tokensJugador),
             panelX + panelW*2/3 + 14, panelY+24, 18, GREEN);

    // --- TÍTULO ---
    float yTitulo = h * 0.36f;
    DrawText("USER LOGIN", w/2 - MeasureText("USER LOGIN", 36)/2, yTitulo, 36, WHITE);
    DrawLine(w*0.12f, yTitulo+42, w*0.88f, yTitulo+42, (Color){0,255,255,100});

    // --- TEXTBOXES ---
    float boxWidth  = w * 0.78f;
    float boxHeight = 48;
    float x = (w - boxWidth) / 2;

    float yUsuario = h * 0.50f;
    float yPass    = h * 0.62f;
    float yBtn     = h * 0.76f;

    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 10);

    // Label + Caja Usuario
    DrawText("USUARIO", x, yUsuario - 18, 11, NEON_CYAN);
    if (GuiTextBox({x, yUsuario, boxWidth, boxHeight}, usuario, 32, usuarioEditMode)) {
        usuarioEditMode  = !usuarioEditMode;
        passwordEditMode = false;
    }
    if (strlen(usuario) == 0 && !usuarioEditMode)
        DrawText("INSERT NAME", x + 12, yUsuario + 15, 18, GRAY);

    // Label + Caja Password
    DrawText("PASSWORD", x, yPass - 18, 11, NEON_CYAN);
    if (GuiTextBox({x, yPass, boxWidth, boxHeight}, password, 32, passwordEditMode)) {
        passwordEditMode = !passwordEditMode;
        usuarioEditMode  = false;
    }
    if (strlen(password) == 0 && !passwordEditMode)
        DrawText("INSERT PASS", x + 12, yPass + 15, 18, GRAY);

    // --- BOTÓN ---
    Rectangle btn = {x, yBtn, boxWidth, 60};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
    DrawRectangleRec(btn, hover ? NEON_CYAN : WHITE);
    DrawText("START GAME", w/2 - MeasureText("START GAME", 26)/2, btn.y + 17, 26, BLACK);

    // --- LOGIN LOGIC ---
    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::string token = loginRequest(usuario, password);
        if (!token.empty()) {
            jwtToken = token;
            tokensAntesLogin = api.getTokens(jwtToken);
            tokensJugador    = tokensAntesLogin;
            RestarTokenLogin();
            loginExitoso = true;
        } else {
            mostrarError  = true;
            mensajeError  = "CREDENTIALS ERROR";
        }
    }

    if (mostrarError)
        DrawText(mensajeError, w/2 - MeasureText(mensajeError,18)/2, h*0.90f, 18, RED);

    DibujarScanlines();
    return loginExitoso;
}

#endif // PRUEBA2_LOGIN_H