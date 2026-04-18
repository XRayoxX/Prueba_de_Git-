#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>

// ============================================================
//  CONFIGURACIÓN Y CONSTANTES
// ============================================================
namespace Config {
    constexpr int   SCREEN_W      = 400;
    constexpr int   SCREEN_H      = 600;
    constexpr float GRAVITY       = 0.45f;
    constexpr float JUMP_SPEED    = -12.5f;
    constexpr float PLAYER_SPEED  = 5.0f;
    constexpr float PLAYER_RADIUS = 15.0f;
    constexpr int   PLATFORM_W    = 60;
    constexpr int   PLATFORM_H    = 15;
    constexpr int   PLATFORM_COUNT= 12; // Suficientes para llenar la pantalla
    constexpr float PLAT_SPACING  = 90.0f;
}

enum class PlatformType { NORMAL, MOVING, FRAGILE };
enum class GameState { LOGIN, INTRO, PLAYING, GAME_OVER };

// ============================================================
//  CLASE PLATAFORMA
// ============================================================
class Platform {
public:
    Rectangle rect;
    PlatformType type;
    bool active;
    float moveDir;
    bool broken;
    int breakTimer;

    Platform(float x, float y, PlatformType t) {
        rect = { x, y, (float)Config::PLATFORM_W, (float)Config::PLATFORM_H };
        type = t;
        active = true;
        moveDir = (GetRandomValue(0, 1) == 0) ? 1.0f : -1.0f;
        broken = false;
        breakTimer = 15;
    }

    void Update() {
        if (type == PlatformType::MOVING) {
            rect.x += moveDir * 2.0f;
            if (rect.x <= 0 || rect.x + rect.width >= Config::SCREEN_W) moveDir *= -1;
        }
        if (broken && active) {
            breakTimer--;
            if (breakTimer <= 0) active = false;
        }
    }

    void Draw() const {
        if (!active) return;
        Color col = (type == PlatformType::NORMAL) ? DARKGREEN :
                    (type == PlatformType::MOVING) ? BLUE : ORANGE;
        if (broken) col = RED;
        DrawRectangleRec(rect, col);
        DrawRectangleLinesEx(rect, 2, Fade(BLACK, 0.3f));
    }
};

// ============================================================
//  CLASE PRINCIPAL DEL JUEGO
// ============================================================
class Game {
private:
    GameState state;
    Vector2 playerPos;
    Vector2 playerSpeed;
    std::vector<Platform> platforms;
    Camera2D camera;

    // Variables de Login y Score
    char username[16];
    int letterCount;
    int score;
    int highScore;

public:
    Game() {
        state = GameState::LOGIN;
        letterCount = 0;
        username[0] = '\0';
        highScore = 0;
        camera = { 0 };
        camera.offset = { Config::SCREEN_W / 2.0f, Config::SCREEN_H / 2.0f };
        camera.zoom = 1.0f;
        InitLevel();
    }

    void InitLevel() {
        score = 0;
        playerPos = { Config::SCREEN_W / 2.0f, Config::SCREEN_H - 100.0f };
        playerSpeed = { 0, 0 };
        camera.target = { Config::SCREEN_W / 2.0f, Config::SCREEN_H / 2.0f };

        platforms.clear();

        // 1. Plataforma inicial garantizada bajo el jugador
        platforms.push_back(Platform(playerPos.x - 30, playerPos.y + 25, PlatformType::NORMAL));

        // 2. Generar resto de plataformas iniciales hacia arriba
        for (int i = 1; i < Config::PLATFORM_COUNT; i++) {
            float rx = (float)GetRandomValue(0, Config::SCREEN_W - Config::PLATFORM_W);
            float ry = (Config::SCREEN_H - 100.0f) - (i * Config::PLAT_SPACING);
            platforms.push_back(Platform(rx, ry, PlatformType::NORMAL));
        }
    }

    void Update() {
        switch (state) {
            case GameState::LOGIN:    UpdateLogin();    break;
            case GameState::INTRO:    UpdateIntro();    break;
            case GameState::PLAYING:  UpdatePlaying();  break;
            case GameState::GAME_OVER:UpdateGameOver(); break;
        }
    }

    // --- LÓGICA DE LOGIN ---
    void UpdateLogin() {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (letterCount < 15)) {
                username[letterCount] = (char)key;
                username[letterCount + 1] = '\0';
                letterCount++;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
            letterCount--;
            username[letterCount] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER) && letterCount > 0) state = GameState::INTRO;
    }

    void UpdateIntro() {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            playerSpeed.y = Config::JUMP_SPEED; // Primer salto
            state = GameState::PLAYING;
        }
    }

    void UpdatePlaying() {
        // Movimiento lateral
        if (IsKeyDown(KEY_LEFT))  playerPos.x -= Config::PLAYER_SPEED;
        if (IsKeyDown(KEY_RIGHT)) playerPos.x += Config::PLAYER_SPEED;

        // Teletransporte lateral (Wrap)
        if (playerPos.x < 0) playerPos.x = Config::SCREEN_W;
        if (playerPos.x > Config::SCREEN_W) playerPos.x = 0;

        // Física
        playerSpeed.y += Config::GRAVITY;
        playerPos.y += playerSpeed.y;

        // Colisión con plataformas (solo al caer)
        if (playerSpeed.y > 0) {
            for (auto& p : platforms) {
                if (p.active && CheckCollisionCircleRec(playerPos, Config::PLAYER_RADIUS, p.rect)) {
                    playerSpeed.y = Config::JUMP_SPEED;
                    if (p.type == PlatformType::FRAGILE) p.broken = true;
                    break;
                }
            }
        }

        // Cámara sigue al jugador solo hacia arriba
        if (playerPos.y < camera.target.y) {
            camera.target.y = playerPos.y;
        }

        // RECICLAJE INFINITO
        float screenBottom = camera.target.y + (Config::SCREEN_H / 2.0f);
        for (auto& p : platforms) {
            p.Update();

            // Si la plataforma queda atrás, la mandamos arriba
            if (p.rect.y > screenBottom) {
                float highestY = screenBottom;
                for (const auto& plat : platforms) if (plat.rect.y < highestY) highestY = plat.rect.y;

                p.rect.y = highestY - Config::PLAT_SPACING;
                p.rect.x = (float)GetRandomValue(0, Config::SCREEN_W - Config::PLATFORM_W);
                p.active = true;
                p.broken = false;
                p.breakTimer = 15;

                // Variedad de tipos
                int rng = GetRandomValue(0, 100);
                if (rng < 20) p.type = PlatformType::MOVING;
                else if (rng < 40) p.type = PlatformType::FRAGILE;
                else p.type = PlatformType::NORMAL;

                score += 10;
            }
        }

        if (score > highScore) highScore = score;

        // Muerte
        if (playerPos.y > screenBottom + 200) state = GameState::GAME_OVER;
    }

    void UpdateGameOver() {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            InitLevel();
            state = GameState::INTRO;
        }
    }

    // --- DIBUJADO ---
    void Draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (state == GameState::LOGIN) {
            DrawText("LOGIN USUARIO", 100, 200, 25, DARKBLUE);
            DrawRectangle(80, 250, 240, 40, LIGHTGRAY);
            DrawText(username, 95, 260, 20, BLACK);
            DrawText("Escribe tu nombre y pulsa ENTER", 80, 310, 15, GRAY);
        }
        else {
            BeginMode2D(camera);
                for (const auto& p : platforms) p.Draw();
                DrawCircleV(playerPos, Config::PLAYER_RADIUS, RED);
                DrawCircleGradient((int)playerPos.x, (int)playerPos.y, 5, WHITE, RED); // Brillo
            EndMode2D();

            // Interfaz (HUD)
            DrawRectangle(0, 0, Config::SCREEN_W, 50, Fade(SKYBLUE, 0.8f));
            DrawText(TextFormat("PLAYER: %s", username), 10, 15, 18, BLACK);
            DrawText(TextFormat("SCORE: %05i", score), 260, 15, 18, MAROON);

            if (state == GameState::INTRO) {
                DrawRectangle(0, 0, Config::SCREEN_W, Config::SCREEN_H, Fade(BLACK, 0.3f));
                DrawText("LISTO?", 150, 250, 30, WHITE);
                DrawText("Presiona ESPACIO para empezar", 80, 300, 15, LIGHTGRAY);
            }

            if (state == GameState::GAME_OVER) {
                DrawRectangle(0, 0, Config::SCREEN_W, Config::SCREEN_H, Fade(BLACK, 0.7f));
                DrawText("GAME OVER", 110, 200, 35, RED);
                DrawText(TextFormat("Puntaje Final: %i", score), 130, 260, 20, WHITE);
                DrawText(TextFormat("Récord: %i", highScore), 150, 290, 18, GOLD);
                DrawText("Pulsa ENTER para volver a intentar", 70, 380, 15, LIGHTGRAY);
            }
        }

        EndDrawing();
    }
};

// ============================================================
//  MAIN
// ============================================================
int main() {
    InitWindow(Config::SCREEN_W, Config::SCREEN_H, "Doodle Jump Grupo 4");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}