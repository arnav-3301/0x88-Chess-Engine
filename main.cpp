#include "raylib.h"
#include "board.h"

const int SQUARE_SIZE = 80;
const int BOARD_SIZE = 8 * SQUARE_SIZE;

int main() {
    InitWindow(BOARD_SIZE, BOARD_SIZE, "0x88 Chess Engine");
    SetTargetFPS(60);

    ChessGame game;

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        ClearBackground(BLACK);

        game.DrawBoard();
        game.DrawMoves();
        game.DrawPieces();
        game.DrawPromotionMenu();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
