#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"

enum Piece { EMPTY = 0, P = 1, N = 2, B = 3, R = 4, Q = 5, K = 6 };

#include <vector>

struct Move {
    int from;
    int to;
    bool isCapture = false;
    bool isEnPassant = false;
    bool isCastling = false;
    int promotionPiece = 0; 
};

class ChessGame {
public:
    int board[128];
    Texture2D whiteTextures[7];
    Texture2D blackTextures[7];

    int selectedSquare;
    int enPassantSquare;
    int sideToMove; // 1 - White, -1 - Black
    
    bool isPromoting;
    Move pendingPromotionMove;

    std::vector<Move> activeMoves;

    // Castling flags
    bool castleWK;
    bool castleWQ;
    bool castleBK;
    bool castleBQ;
    bool isCheckmate;
    bool isStalemate;

    ChessGame();
    ~ChessGame();

    void InitStartingPosition();
    bool isSquareOnBoard(int index);
    bool isSquareAttacked(int square, int attackingColorSign);
    void LoadPieceTextures();
    void UnloadPieceTextures();

    std::vector<Move> GeneratePseudoLegalMoves(int colorToGenerate);
    std::vector<Move> GenerateLegalMoves(int colorToGenerate);

    void MakeMove(Move m);
    void Update();
    void DrawBoard();
    void DrawPieces();
    void DrawPromotionMenu();
    void DrawMoves();
    void CheckForGameOver();
    void DrawGameOver();
};

#endif
