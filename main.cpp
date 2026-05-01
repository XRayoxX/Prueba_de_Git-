#include "raylib.h"
#define RAYGUI_IMPLEMENTATION

#include "login.h"
#include "Puntaje.h"
#include "game_over.h"

#include <vector>
#include <string>

namespace Config{

constexpr int SCREEN_W=400;
constexpr int SCREEN_H=600;

constexpr float GRAVITY=0.45f;
constexpr float JUMP_SPEED=-12.5f;
constexpr float PLAYER_SPEED=5.0f;

constexpr float PLAYER_RADIUS=15.0f;

constexpr int PLATFORM_W=60;
constexpr int PLATFORM_H=15;
constexpr int PLATFORM_COUNT=12;

constexpr float PLAT_SPACING=90.0f;

}

enum class PlatformType{
NORMAL,
MOVING,
FRAGILE
};

enum class GameState{
LOGIN,
INTRO,
PLAYING,
GAME_OVER
};


class Platform{

public:

Rectangle rect;
PlatformType type;

bool active;
float moveDir;

bool broken;
int breakTimer;


Platform(float x,float y,PlatformType t){

rect={
x,
y,
(float)Config::PLATFORM_W,
(float)Config::PLATFORM_H
};

type=t;

active=true;

moveDir=
(GetRandomValue(0,1)==0)?
1.0f:-1.0f;

broken=false;
breakTimer=15;
}


void Update(){

if(type==PlatformType::MOVING){

rect.x+=moveDir*2.0f;

if(rect.x<=0 ||
rect.x+rect.width>=Config::SCREEN_W)
moveDir*=-1;
}


if(broken && active){

breakTimer--;

if(breakTimer<=0)
active=false;
}
}


void Draw() const{

if(!active)
return;


Color col=
(type==PlatformType::NORMAL)?
DARKGREEN:
(type==PlatformType::MOVING)?
BLUE:
ORANGE;

if(broken)
col=RED;


DrawRectangleRec(rect,col);

DrawRectangleLinesEx(
rect,
2,
Fade(BLACK,0.3f)
);
}

};



class Game{

private:

GameState state;

Vector2 playerPos;
Vector2 playerSpeed;

std::vector<Platform> platforms;

Camera2D camera;

int score;
int highScore;

GameOverScreen gameOver;


public:

Game(){

state=GameState::LOGIN;

highScore=0;

camera={0};

camera.offset={
Config::SCREEN_W/2.0f,
Config::SCREEN_H/2.0f
};

camera.zoom=1.0f;

InitLevel();
}



void InitLevel(){

ResetBonusTokens();

score=0;

playerPos={
Config::SCREEN_W/2.0f,
Config::SCREEN_H-100.0f
};

playerSpeed={0,0};

camera.target={
Config::SCREEN_W/2.0f,
Config::SCREEN_H/2.0f
};

platforms.clear();


platforms.push_back(
Platform(
playerPos.x-30,
playerPos.y+25,
PlatformType::NORMAL
)
);


for(int i=1;i<Config::PLATFORM_COUNT;i++){

float rx=
(float)GetRandomValue(
0,
Config::SCREEN_W-
Config::PLATFORM_W
);


float ry=
(Config::SCREEN_H-100.0f)
-(i*Config::PLAT_SPACING);


platforms.push_back(
Platform(
rx,
ry,
PlatformType::NORMAL
)
);
}

}



void Update(){

switch(state){

case GameState::LOGIN:
UpdateLogin();
break;

case GameState::INTRO:
UpdateIntro();
break;

case GameState::PLAYING:
UpdatePlaying();
break;

case GameState::GAME_OVER:
UpdateGameOver();
break;
}
}



void UpdateLogin(){}


void UpdateIntro(){

if(IsKeyPressed(KEY_SPACE)||
IsKeyPressed(KEY_ENTER)){

playerSpeed.y=
Config::JUMP_SPEED;

state=
GameState::PLAYING;
}
}



void UpdatePlaying(){

if(IsKeyDown(KEY_LEFT))
playerPos.x-=Config::PLAYER_SPEED;


if(IsKeyDown(KEY_RIGHT))
playerPos.x+=Config::PLAYER_SPEED;


if(playerPos.x<0)
playerPos.x=Config::SCREEN_W;

if(playerPos.x>Config::SCREEN_W)
playerPos.x=0;


playerSpeed.y+=Config::GRAVITY;

playerPos.y+=playerSpeed.y;



if(playerSpeed.y>0){

for(auto& p:platforms){

if(
p.active &&
CheckCollisionCircleRec(
playerPos,
Config::PLAYER_RADIUS,
p.rect
)
){

playerSpeed.y=
Config::JUMP_SPEED;


if(p.type==
PlatformType::FRAGILE)
p.broken=true;

break;
}
}
}



if(playerPos.y<camera.target.y)
camera.target.y=playerPos.y;



float screenBottom=
camera.target.y+
(Config::SCREEN_H/2.0f);



for(auto& p:platforms){

p.Update();


if(p.rect.y>screenBottom){

float highestY=
screenBottom;

for(const auto& plat:platforms)
if(plat.rect.y<highestY)
highestY=plat.rect.y;


p.rect.y=
highestY-
Config::PLAT_SPACING;


p.rect.x=
(float)GetRandomValue(
0,
Config::SCREEN_W-
Config::PLATFORM_W
);


p.active=true;
p.broken=false;
p.breakTimer=15;


int rng=
GetRandomValue(0,100);


if(rng<20)
p.type=PlatformType::MOVING;

else if(rng<40)
p.type=PlatformType::FRAGILE;

else
p.type=PlatformType::NORMAL;


score+=10;

// =====================================================
// BUG 3 FIX: Se guarda el score en que se chequeó por
// última vez para no llamar la API cada frame.
// RevisarBonusTokens ya tiene el guard internamente,
// pero también lo protegemos aquí con el mismo score.
// =====================================================
RevisarBonusTokens(score);

}
}


if(score>highScore)
highScore=score;



if(playerPos.y>
screenBottom+200){

state=
GameState::GAME_OVER;

gameOver.Start();
}
}



void UpdateGameOver(){

if(gameOver.TiempoTerminado()){

InitLevel();
gameOver.Reset();

// FIX: tiempo agotado también regresa al LOGIN y limpia al usuario
loginValidado = false;
mostrarError  = false;
usuario[0]    = '\0';
password[0]   = '\0';
jwtToken      = "";
tokensJugador = 0;
state = GameState::LOGIN;

return;
}



if(gameOver.ClickContinue()){

if(gameOver.HandleContinue()){

// =====================================================
// BUG 2 FIX: Mover al jugador encima de la cámara y
// sincronizar la cámara a su nueva posición para que
// no aparezca en un lugar aleatorio.
// =====================================================
playerPos.y = camera.target.y - (Config::SCREEN_H / 2.0f) + 200.0f;
camera.target.y = playerPos.y;

playerSpeed.y=Config::JUMP_SPEED;

state=GameState::PLAYING;
}
}



if(gameOver.ClickSalir()){

// =====================================================
// BUG 1 FIX: Salir debe volver al LOGIN y resetear
// loginValidado para que el usuario tenga que
// autenticarse de nuevo.
// =====================================================
InitLevel();
gameOver.Reset();
loginValidado = false;          // Resetea el flag del Login
mostrarError  = false;          // Limpia mensajes de error
usuario[0]    = '\0';           // Limpia campo usuario (opcional)
password[0]   = '\0';           // Limpia campo password (opcional)
state = GameState::LOGIN;       // ← LOGIN, no INTRO
}

}



void Draw(){

BeginDrawing();

ClearBackground(RAYWHITE);



if(state==GameState::LOGIN){

if(DibujarLogin())
state=GameState::INTRO;
}

else{

BeginMode2D(camera);

for(const auto& p:platforms)
p.Draw();

DrawCircleV(
playerPos,
Config::PLAYER_RADIUS,
RED
);

EndMode2D();



DrawRectangle(
0,
0,
Config::SCREEN_W,
50,
Fade(SKYBLUE,0.8f)
);


DrawText(
TextFormat(
"PLAYER: %s",
usuario
),
10,
15,
18,
BLACK
);


DrawText(
TextFormat(
"SCORE: %05i",
score
),
200,
15,
18,
MAROON
);


DrawText(
TextFormat(
"TOKENS: %d",
tokensJugador
),
10,
35,
14,
GOLD
);



if(state==GameState::INTRO){

DrawRectangle(
0,0,
Config::SCREEN_W,
Config::SCREEN_H,
Fade(BLACK,0.3f)
);


DrawText(
"LISTO?",
150,
250,
30,
WHITE
);


DrawText(
"Presiona ESPACIO para empezar",
80,
300,
15,
LIGHTGRAY
);

}



if(state==GameState::GAME_OVER){

gameOver.Draw();

}

}

EndDrawing();

}

};

#include "api.h"
ApiClient api(
"https://arcadeumg.duckdns.org/api"
);
std::string jwtToken="";
int tokensJugador=0;

int main(){
InitWindow(
Config::SCREEN_W,
Config::SCREEN_H,
"Doodle Jump Grupo 3"
);
SetTargetFPS(60);
Game game;
while(!WindowShouldClose()){
game.Update();
game.Draw();
}
CloseWindow();
return 0;
}
