#ifndef PRUEBA2_LOGIN_H
#define PRUEBA2_LOGIN_H
#include "raylib.h"
#include "raygui.h" // <--- ¡Esto es lo que faltaba!
#include <string.h>
#include  "requests.h"
#include "api.h"
// Variables de estado
inline char usuario[32] = "";
inline char password[32] = "";
inline bool passwordEditMode = false;
inline bool usuarioEditMode = false;
inline bool mostrarError = false;
inline const char* mensajeError = "";

// Colores Retro Neon
static const Color NEON_CYAN = { 0, 255, 255, 255 };
static const Color NEON_PINK = { 255, 0, 255, 255 };
static const Color RETRO_BLUE = { 10, 20, 40, 255 };
static const Color GRID_COLOR = { 0, 100, 150, 100 };

// Función para dibujar el fondo de rejilla (Grid) en perspectiva
void DibujarFondoRetro() {
    ClearBackground(RETRO_BLUE);
    float w = GetScreenWidth();
    float h = GetScreenHeight();

    // Dibujar el "suelo" de rejilla
    for (int i = 0; i <= 20; i++) {
        // Líneas horizontales (perspectiva)
        float y = h * 0.7f + (i * i * 0.5f);
        if (y < h) DrawLine(0, y, w, y, GRID_COLOR);

        // Líneas verticales que convergen al centro
        DrawLine(w / 2, h * 0.6f, (i * w / 10) - w/2, h, GRID_COLOR);
    }

    // Efecto de resplandor en el horizonte
    DrawRectangleGradientV(0, h * 0.4f, w, h * 0.3f, Fade(BLACK, 0.0f), NEON_CYAN);
}

// Función para simular líneas de TV vieja (Scanlines)
void DibujarScanlines() {
    for (int i = 0; i < GetScreenHeight(); i += 4) {
        DrawLine(0, i, GetScreenWidth(), i, (Color){ 0, 0, 0, 40 });
    }
}

inline bool DibujarLogin() {
    bool loginExitoso = false;
    float w = GetScreenWidth();
    float h = GetScreenHeight();

    DibujarFondoRetro();

    // --- ENCABEZADO ARCADE ---
    DrawText("1P", 20, 20, 25, WHITE);
    DrawText("00", 20, 50, 25, NEON_CYAN);

    DrawText("HI-SCORE", w/2 - 50, 20, 20, YELLOW);
    DrawText("999999", w/2 - 45, 45, 25, WHITE);

    DrawText("Puntos", w - 20, 10, 25, WHITE);
    DrawText("00", w - 50, 50, 25, NEON_PINK);

    // Título Principal con "Glow"
    DrawText("USER LOGIN", w/2 - MeasureText("USER LOGIN", 40)/2 + 3, h * 0.15f + 3, 40, NEON_PINK);
    DrawText("USER LOGIN", w/2 - MeasureText("USER LOGIN", 40)/2, h * 0.15f, 40, WHITE);

    // --- CAMPOS DE TEXTO ESTILO RETRO ---
    float ancho = w * 0.7f;
    float x = w/2 - ancho/2;

    // Caja Usuario
    DrawRectangleLinesEx((Rectangle){ x, h * 0.35f, ancho, 50 }, 2, NEON_CYAN);
    if (usuarioEditMode) DrawRectangleRec((Rectangle){ x + 5, h * 0.35f + 5, ancho - 10, 40 }, Fade(NEON_CYAN, 0.2f));

    // Icono Usuario (Simple pixel art)
    DrawRectangle(x + 10, h * 0.35f + 10, 30, 30, WHITE);

    // Usamos Raygui de forma invisible sobre nuestro diseño
    if (GuiTextBox((Rectangle){ x + 50, h * 0.35f, ancho - 60, 50 }, usuario, 32, usuarioEditMode)) {
        usuarioEditMode = !usuarioEditMode;
        passwordEditMode = false;
    }
    if (strlen(usuario) == 0 && !usuarioEditMode) DrawText("INSERT NAME", x + 60, h * 0.35f + 15, 20, GRAY);

    // Caja Password
    DrawRectangleLinesEx((Rectangle){ x, h * 0.50f, ancho, 50 }, 2, NEON_PINK);
    if (passwordEditMode) DrawRectangleRec((Rectangle){ x + 5, h * 0.50f + 5, ancho - 10, 40 }, Fade(NEON_PINK, 0.2f));

    // Icono Llave (Simple pixel art)
    DrawRectangle(x + 10, h * 0.50f + 10, 30, 30, WHITE);

    if (GuiTextBox((Rectangle){ x + 50, h * 0.50f, ancho - 60, 50 }, password, 32, passwordEditMode)) {
        passwordEditMode = !passwordEditMode;
        usuarioEditMode = false;
    }
    if (strlen(password) == 0 && !passwordEditMode) DrawText("INSERT PASS", x + 60, h * 0.50f + 15, 20, GRAY);

    // --- BOTÓN START (LOGIN) ---
    Rectangle btnRect = { x, h * 0.70f, ancho, 60 };
    bool mouseSobre = CheckCollisionPointRec(GetMousePosition(), btnRect);

    // Efecto parpadeo estilo "Press Start"
    if (!mouseSobre || (int)(GetTime()*4)%2 == 0) {
        DrawRectangleRec(btnRect, mouseSobre ? WHITE : NEON_CYAN);
        DrawText("START GAME", btnRect.x + ancho/2 - MeasureText("START GAME", 25)/2, btnRect.y + 18, 25, BLACK);
    }

    if (mouseSobre && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        std::string token = loginRequest(usuario, password);

        if (!token.empty()) {
            loginExitoso = true;
            mostrarError = false;
        } else {
            mostrarError = true;
            mensajeError = "CREDENTIALS ERROR";
        }
    }

    if (mostrarError) {
        DrawText(mensajeError, w/2 - MeasureText(mensajeError, 20)/2, h * 0.85f, 20, RED);
    }


    DibujarScanlines(); // Capa final de efecto TV
    return loginExitoso;
}

#endif