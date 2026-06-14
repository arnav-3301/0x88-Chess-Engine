#include "raylib.h"
#include "board.h"

const int SQUARE_SIZE = 80;
const int BOARD_SIZE = 8 * SQUARE_SIZE;

int main() {
    // 1. ALWAYS initialize the window first so the GPU context exists
    InitWindow(640, 640, "0x88 Chess Engine");
    SetTargetFPS(60);

    // 2. NOW it is safe to create the game (and load textures in the background)
    ChessGame game;

    // 3. Run the Perft test
    game.RunPerftTest(5);

    // 4. Safely close and exit
    CloseWindow();
    return 0;

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        ClearBackground(BLACK);

        game.DrawBoard();
        game.DrawMoves();
        game.DrawPieces();
        game.DrawPromotionMenu();
        game.DrawGameOver();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
