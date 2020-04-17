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
*   Copyright (c) 2020 <TODO: participant_name> (<TODO: participant_github_name>)
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

//TODO ADD SOUND
//TODO ADD START AND END SCREEN AND save HIGH SCORE with that raylib func

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

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

int pipeDistanceTop, pipeDistanceBottom = 0;
int frameCount = 0;
int score = 0;
int i;

enum GameScreen { 
    TITLE, 
    GAMEPLAY
};

enum Difficulty {
    EASY,
    MEDIUM,
    IMPOSSIBLE
};

struct Player {
    Rectangle position;
    Color color;
    bool isJumping;
    bool isDead;
    bool isColliding;
};

struct Pipe {
    int id;
    Rectangle position;
    Color color;
    bool hasCollided;
};
struct Pipe pipes[NUMBER_OF_PIPES];

bool isColliding(Rectangle *rec1, Rectangle *rec2)
{
    bool collision = false;

    if ((rec1->x < (rec2->x + rec2->width) && (rec1->x + rec1->width) > rec2->x) &&
        (rec1->y < (rec2->y + rec2->height) && (rec1->y + rec1->height) > rec2->y)) collision = true;

    return collision;
}



void initialize() {   //inits once
    
}

void updateFrame() { //updates each frame

} 

void drawFrame() {  //draws each frame

   
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
   
    for (i = 0; i < NUMBER_OF_PIPES; i++) {
        //even top pipes
        if (i % 2 == 0) { //even, starts with 0, 2, 4 etc
        pipes[i].id = i;
        pipes[i].position.x = (float)30 + pipeDistanceTop;
        pipes[i].position.y = (float)0;
        pipes[i].position.width = 2.0f;
        pipes[i].position.height = (float)GetRandomValue(4,12);
        pipes[i].color = GREEN;
        pipes[i].hasCollided = false;
        pipeDistanceTop += 12;
        printf("Even Pipe %d\n", pipes[i].id);
        }
    }
    
    for (i = 0; i < NUMBER_OF_PIPES; i++) {
        //bottom odd pipes
        if (i % 2 != 0) { //odd 1, 3, 5 etc
            pipes[i].id = i;
            pipes[i].position.x = (float)30 + pipeDistanceBottom;
            pipes[i].position.y = (float)(pipes[i - 1].position.height + EASY_HEIGHT);
            pipes[i].position.width = 2.0f;
            pipes[i].position.height = 18.0f;
            pipes[i].color = GREEN;
            pipes[i].hasCollided = false;
            pipeDistanceBottom += 12;
            printf("Odd Pipe %d\n", pipes[i].id);
        }
    }
   
    struct Player player = { (gameScreenWidth - PLAYER_WIDTH) / 2 ,
                             (gameScreenHeight - PLAYER_HEIGHT) / 2,
                              PLAYER_WIDTH, PLAYER_HEIGHT, GOLD, false, false, false };
    
   
    // ------------ end of INIT CODE ------------------

    SetTargetFPS(FPS);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update--------------------->


        if (!player.isDead) { //while not dead

            //player falls
            (float)player.position.y += GRAVITY;

            //detecting player input and applying velocity
            if (IsKeyPressed(KEY_SPACE)) {
                for (i = 0; i < COUNT; i++) {
                    player.position.y -= JUMP_POWER;
                    player.isJumping = true;
                }
            }//end is space pressed

            //pipes move
            for (i = 0; i < NUMBER_OF_PIPES; i++) {
                frameCount++;
    //debugging frames printf("Frames: %d\n", frameCount);
                pipes[i].position.x -= VELOCITY;
                //reset pipe
                if (pipes[i].position.x <= -4) {
                    pipes[i].position.x = 32 + 12;
                    if (i % 2 == 0) pipes[i].position.height = GetRandomValue(4, 12);
                    if (i % 2 != 0) pipes[i].position.y = (float)(pipes[i - 1].position.height + EASY_HEIGHT);
                }
                if ((int)player.position.x == ((int)pipes[i].position.x + 1)) {
                    if (frameCount >= 1000) {
                        score++;
      //debugging score printf("Score: %d\n", score);
                        frameCount = 0;
                    }
                }

           
            }//end for
             
            //collision detection
            //top
            if (player.position.y <= 0) {
                player.position.y = 0;
                printf("DEAD at Y Position:%d \n", player.position.y);
                player.isDead = true;
            }
            //bottom
            if (player.position.y >= 30) {
                printf("DEAD at Y Position:%d \n", player.position.y);
                player.isDead = true;
            }
            //pipes
            for (i = 0; i < NUMBER_OF_PIPES; i++) {
                if (isColliding(&player.position, &pipes[i].position)) {
                    printf("DEAD on pipe[%d], pipe x:%d, pipe y:%d \n", pipes[i].id, (int)pipes[i].position.x, (int)pipes[i].position.y);
                    player.isDead = true;
                }
            }

        }//----> end if not dead


         
        
   

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

        DrawTexture(skyTexture, 0, 0, WHITE);
        
        for (i = 0; i < NUMBER_OF_PIPES; i++) {
            DrawRectangle(pipes[i].position.x, pipes[i].position.y, pipes[i].position.width, pipes[i].position.height, pipes[i].color);
        }
        
        DrawRectangle((int)player.position.x, (int)player.position.y, player.position.width, player.position.height, player.color);

        //
       
        

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

        DrawText(TextFormat("%d", score), 246, 50, 55, BLACK);
        DrawText(TextFormat("%d", score), 246, 50, 53, WHITE);
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
