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
    DrawText("1P", 35, 20, 25, WHITE);
    DrawText("00", 35, 50, 25, NEON_CYAN);

    DrawText("PLAYER", w/2 - MeasureText("PLAYER", 20)/2, 20, 20, YELLOW);
    DrawText(usuario, w/2 - MeasureText(usuario, 25)/2, 45, 25, WHITE);

    DrawText("Puntos", w - 130, 22, 20, WHITE);
    DrawText("00", w - 90, 50, 25, NEON_PINK);

    // --- SECCIÓN DE TOKENS (CENTRADA) ---
    DrawText("TOKENS", w/2 - MeasureText("TOKENS", 20)/2, 80, 20, NEON_CYAN);
    DrawText(TextFormat("%d", tokensJugador), w/2 - MeasureText(TextFormat("%d", tokensJugador), 25)/2, 105, 25, WHITE);

    // Movimos estos textos al centro y ajustamos su posición Y
    DrawText(TextFormat("Antes: %d", tokensAntesLogin), w/2 - MeasureText(TextFormat("Antes: %d", tokensAntesLogin), 18)/2, 140, 18, GOLD);
    DrawText(TextFormat("Descuento: -%d", tokensDescontados), w/2 - MeasureText(TextFormat("Descuento: -%d", tokensDescontados), 18)/2, 165, 18, RED);
    DrawText(TextFormat("Actual: %d", tokensJugador), w/2 - MeasureText(TextFormat("Actual: %d", tokensJugador), 18)/2, 190, 18, GREEN);

    // --- TÍTULO PRINCIPAL (Bajado para no chocar) ---
    float yTitulo = h * 0.28f; // Bajado de 0.22f a 0.28f
    DrawText("USER LOGIN", w/2 - MeasureText("USER LOGIN", 40)/2, yTitulo, 40, WHITE);

    // --- TEXTBOXES (Bajados proporcionalmente) ---
    float boxWidth = w * 0.75f;
    float boxHeight = 50;
    float x = (w - boxWidth) / 2;

    float yUsuario = h * 0.40f; // Antes 0.35f
    float yPass    = h * 0.52f; // Antes 0.47f
    float yBtn     = h * 0.68f; // Antes 0.62f

    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 10);

    // Caja Usuario
    if (GuiTextBox({x, yUsuario, boxWidth, boxHeight}, usuario, 32, usuarioEditMode)) {
        usuarioEditMode = !usuarioEditMode;
        passwordEditMode = false;
    }
    if (strlen(usuario) == 0 && !usuarioEditMode)
        DrawText("INSERT NAME", x + 10, yUsuario + 15, 20, GRAY);

    // Caja Password
    if (GuiTextBox({x, yPass, boxWidth, boxHeight}, password, 32, passwordEditMode)) {
        passwordEditMode = !passwordEditMode;
        usuarioEditMode = false;
    }
    if (strlen(password) == 0 && !passwordEditMode)
        DrawText("INSERT PASS", x + 10, yPass + 15, 20, GRAY);

    // --- BOTÓN ---
    Rectangle btn = {x, yBtn, boxWidth, 65};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
    DrawRectangleRec(btn, hover ? NEON_CYAN : WHITE);
    DrawText("START GAME", w/2 - MeasureText("START GAME", 28)/2, btn.y + 18, 28, BLACK);

    // --- LÓGICA DE LOGIN ---
    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::string token = loginRequest(usuario, password);
        if (!token.empty()) {
            jwtToken = token;
            tokensAntesLogin = api.getTokens(jwtToken);
            tokensJugador = tokensAntesLogin;
            RestarTokenLogin();
            loginExitoso = true;
        } else {
            mostrarError = true;
            mensajeError = "CREDENTIALS ERROR";
        }
    }

    if (mostrarError) {
        DrawText(mensajeError, w/2 - MeasureText(mensajeError, 20) / 2, h * 0.85f, 20, RED);
    }

    DibujarScanlines();
    return loginExitoso;
}

#endif