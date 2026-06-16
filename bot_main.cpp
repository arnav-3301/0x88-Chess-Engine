#include "board.h"
#include <raylib.h>

int main() {
    InitWindow(1000, 700, "0x88 Chess - Bot Arena");
    SetTargetFPS(60);

    ChessGame game(false);

    float moveTimer = 0.0f;
    const float BOT_DELAY = 1.0f;

    game.isAutoPlay = true;
    game.botSearchDepth = 4;

    while (!WindowShouldClose()) {

        // 1. Only run the timer if the game is still active
        if (!game.isCheckmate && !game.isStalemate) {

            // GetFrameTime() returns the time (in seconds) since the last frame was drawn
            moveTimer += GetFrameTime(); 

            // 2. When 1 second has passed, let the bot calculate and move
            if (moveTimer >= BOT_DELAY) {

                game.AutoPlayUpdate(); 

                moveTimer = 0.0f; // Reset the clock for the next turn
            }
        }

        // 3. Render the graphics at a smooth 60 FPS while the timer counts up
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF)); 

        game.DrawBoard();
        game.DrawCoordinates();
        game.DrawMoves();
        game.DrawPieces();

        game.DrawSidebar(); 
        game.DrawGameOver();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
