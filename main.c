/**********************************************************************************************
*
*   raylib 32x32 game/demo competition
*
*   Competition consist in developing a videogame in a 32x32 pixels screen size.
*
*   RULES:
*
*     1) Use only raylib (and included libraries), no external libraries allowed
*     2) The submission should consist of just one source file
*     3) Render your game/demo to a 32x32 pixels render texture,
*        show what you could do with a 32x32 RGB LED matrix!
*     4) No external resources, you CAN only create them programmatically,
*     5) Game/demo can be 2D or 3D, choose wisely
*     5) Shaders (if used) should be included in the source as string (char *)
*        and loaded with LoadShaderCode()
*     6) Code must compile and execute in PLATFORM_DESKTOP (Windows, Linux, macOS)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020 Rudy Faile (@rfaile313)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "raylib.h"
#include "raymath.h"

#define max(a, b) ((a)>(b)? (a) : (b))
#define min(a, b) ((a)<(b)? (a) : (b))

#define MAX_PIXELS 1024
#define PLAYER_WIDTH 2.0f
#define PLAYER_HEIGHT 2.0f
#define NUMBER_OF_PIPES 16

#define EASY_HEIGHT 18
#define MEDIUM_HEIGHT 14
#define IMPOSSIBLE_HEIGHT 8

#define FPS 60


const int windowWidth = 512;        // NOTE: Game will be scaled x16
const int windowHeight = 512;
const int gameScreenWidth = 32;
const int gameScreenHeight = 32;

const float GRAVITY = 0.2f;
const float JUMP_POWER = 0.05f;
const float VELOCITY = 0.15f;
const int COUNT = 150;

bool title = true;
bool scoreScreen = false;

int pipeDistanceTop, pipeDistanceBottom = 0;
int currentDifficulty = EASY_HEIGHT;
int frameCount = 0;
int score = 0;
int hiScore;
int i;

typedef enum {
    STORAGE_POSITION_HISCORE = 0
} StorageData;

struct Player {
    Rectangle position;
    Color color;
    bool isDead;
};

struct Pipe {
    int id;
    Rectangle position;
    Color color;
};

Vector2 title_pos = { 0, 1 };
Vector2 box_pos = { 7, 11 };
Vector2 score_pos = { 13, 1 };
Vector2 score_text = { 0, 5 };
Vector2 hi_score_text = { 0, 16 };

bool isColliding(Rectangle *rec1, Rectangle *rec2)
{
    bool collision = false;

    if ((rec1->x < (rec2->x + rec2->width) && (rec1->x + rec1->width) > rec2->x) &&
        (rec1->y < (rec2->y + rec2->height) && (rec1->y + rec1->height) > rec2->y)) collision = true;

    return collision;
}

struct Pipe pipes[NUMBER_OF_PIPES];

void generatePipes() {
    
    for (i = 0; i < NUMBER_OF_PIPES; i++) {
        //even top pipes
        if (i % 2 == 0) { //even, starts with 0, 2, 4 etc
            pipes[i].id = i;
            pipes[i].position.x = (float)30 + pipeDistanceTop;
            pipes[i].position.y = (float)0;
            pipes[i].position.width = 2.0f;
            pipes[i].position.height = (float)GetRandomValue(4, 12);
            pipes[i].color = GREEN;
            pipeDistanceTop += 12;
        }
    }

    for (i = 0; i < NUMBER_OF_PIPES; i++) {
        //bottom odd pipes
        if (i % 2 != 0) { //odd 1, 3, 5 etc
            pipes[i].id = i;
            pipes[i].position.x = (float)30 + pipeDistanceBottom;
            pipes[i].position.y = (float)(pipes[i - 1].position.height + currentDifficulty);
            pipes[i].position.width = 2.0f;
            pipes[i].position.height = 18.0f;
            pipes[i].color = GREEN;
            pipeDistanceBottom += 12;
        }
    }

}


int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Rudy 32x32 Raylib game/demo competition");

    // Render texture initialization, used to hold the rendering
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, FILTER_POINT);  // Texture scale filter to use!

    // ------------ INIT CODE here -------------

    Image skyGradient = GenImageGradientV(gameScreenWidth, gameScreenHeight, BLUE, RED);
    Texture2D skyTexture;
    skyTexture = LoadTextureFromImage(skyGradient);
   
    generatePipes();
   
    struct Player player = { (gameScreenWidth - PLAYER_WIDTH) / 2 ,
                             (gameScreenHeight - PLAYER_HEIGHT) / 2,
                              PLAYER_WIDTH, PLAYER_HEIGHT, GOLD, false};
    
   
    // ------------ end of INIT CODE ------------------

    SetTargetFPS(FPS);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update--------------------->

        if (score <= 20) currentDifficulty = EASY_HEIGHT;
        if (score >= 21) currentDifficulty = MEDIUM_HEIGHT;
        if (score >= 40) currentDifficulty = IMPOSSIBLE_HEIGHT;

        hiScore = LoadStorageValue(STORAGE_POSITION_HISCORE);
        if (score >= hiScore) {
            hiScore = score;
            SaveStorageValue(STORAGE_POSITION_HISCORE, hiScore);
        }
           
        if (title && !scoreScreen) {
            //start game w/space
            if (IsKeyPressed(KEY_SPACE)) title = false;   
        }

        if (!player.isDead && !title) { //while not dead && not title screen

            //player falls
            player.position.y += GRAVITY;

            //detecting player input and applying velocity
            if (IsKeyPressed(KEY_SPACE)) {
                for (i = 0; i < COUNT; i++) {
                    player.position.y -= JUMP_POWER;
                }
            }//end is space pressed

            //pipes move
            for (i = 0; i < NUMBER_OF_PIPES; i++) {
                frameCount++;
                pipes[i].position.x -= VELOCITY;
                //reset pipe
                if (pipes[i].position.x <= -4) {
                    pipes[i].position.x = 32 + 12;
                    if (i % 2 == 0) pipes[i].position.height = GetRandomValue(4, 12);
                    if (i % 2 != 0) pipes[i].position.y = (float)(pipes[i - 1].position.height + currentDifficulty);
                }
                if ((int)player.position.x == ((int)pipes[i].position.x + 1)) {
                    if (frameCount >= 1000) {
                        score++;
                        frameCount = 0;
                    }
                }
            }//end for
             
            //collision detection
            //top
            if (player.position.y <= 0) {
                player.isDead = true;
            }
            //bottom
            if (player.position.y >= 30) {
                player.isDead = true;
            }
            //pipes
            for (i = 0; i < NUMBER_OF_PIPES; i++) {
                if (isColliding(&player.position, &pipes[i].position)) {
                    player.isDead = true;
                }
            }

        }//----> end if not dead

        if (player.isDead) {
            //TODO: give it a second to register before allowing space input or doesnt stop on roof
           
            scoreScreen = true;
            if (IsKeyPressed(KEY_SPACE)) {
                //reset everything
                score = 0;
                pipeDistanceTop = 0;
                pipeDistanceBottom = 0;
                currentDifficulty = EASY_HEIGHT;
                frameCount = 0;
                score = 0;
                hiScore;
                i = 0;
                generatePipes();
                player.position.x = (gameScreenWidth - PLAYER_WIDTH) / 2;
                player.position.y = (gameScreenHeight - PLAYER_HEIGHT) / 2;
                player.isDead = false;
                scoreScreen = false;
            }
        }


        //------------------->Update
        
        // Compute required framebuffer scaling
        float scale = min((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
        //----------------------------------------------------------------------------------
    

        // Draw-------------------->

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);
        ClearBackground(BLACK);         // Clear render texture background color

        // ------------ Draw CODE here -------------

        //background
        DrawTexture(skyTexture, 0, 0, WHITE);

        
        if (!title) {
            //pipes
            for (i = 0; i < NUMBER_OF_PIPES; i++) {
                DrawRectangle(pipes[i].position.x, pipes[i].position.y, pipes[i].position.width, pipes[i].position.height, pipes[i].color);
            }
            //player
            DrawRectangle((int)player.position.x, (int)player.position.y, player.position.width, player.position.height, player.color);
        }
        //
      
        if (title) {
            DrawRectangle(
                (gameScreenWidth - PLAYER_WIDTH) / 2, //X
                (gameScreenHeight - PLAYER_HEIGHT) / 2, //Y
                2, 2, GOLD); //w, h, color
        }
        
        if (!title && !scoreScreen) {
            DrawTextEx(GetFontDefault(), TextFormat("%d", score), score_pos, 9.5f, 1.0f, WHITE);
            
        }
        if (title && !scoreScreen) {
            DrawTextEx(GetFontDefault(), "Flappy", title_pos, 9.5f, 1.0f, WHITE);
            DrawTextEx(GetFontDefault(), "Box", box_pos, 9.5f, 1.0f, WHITE);
         
        }
        if (scoreScreen) {
            DrawTextEx(GetFontDefault(), TextFormat("Scre%d", score), score_text, 9.5f, 1.0f, WHITE);
            DrawTextEx(GetFontDefault(), TextFormat("Best%d", hiScore), hi_score_text, 9.5f, 1.0f, WHITE);

        }


        // ------------ end of Draw CODE ------------------

        EndTextureMode();

        // Draw render texture to window, properly scaled
        DrawTexturePro(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle) {
            (GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5,
                (float)gameScreenWidth* scale, (float)gameScreenHeight* scale
        }, (Vector2) { 0, 0 }, 0.0f, WHITE);

        // Draw the grid like "stencil" is drawn over the squares to make them look not at all like LEDs!
        for (int x = 0; x < GetScreenWidth(); x += 16) DrawRectangle(x, 0, 4, GetScreenHeight(), BLACK);
        for (int y = 0; y < GetScreenHeight(); y += 16) DrawRectangle(0, y, GetScreenWidth(), 4, BLACK);

    
        EndDrawing();
        //--------------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);    // Unload render texture
  
    
    

    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

