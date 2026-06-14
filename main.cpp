#include "raylib.h"
#include "board.h"

const int SQUARE_SIZE = 80;
const int BOARD_SIZE = 8 * SQUARE_SIZE;

int main() {
    // ALWAYS initialize the window first so the GPU context exists
    InitWindow(640, 640, "0x88 Chess Engine");
    SetTargetFPS(60);

    ChessGame game;
    // Loading a test FEN
    game.LoadFromFEN("6k1/5ppp/8/8/8/8/5PPP/4R1K1 w - - 0 1");

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
