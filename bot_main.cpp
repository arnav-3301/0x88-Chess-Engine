#include "board.h"
#include <raylib.h>

int main() {
    InitWindow(1000, 700, "0x88 Chess - Bot Arena");
    SetTargetFPS(60);

    ChessGame game(false);
    game.isAutoPlay = true; // Force the UI to show "BOT VS BOT"
    game.botSearchDepth = 4; // Set your tournament depth here

    while (!WindowShouldClose()) {
        // --- NEW: Use the click-free bot logic ---
        game.AutoPlayUpdate(); 

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
