#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "raylib.h"

enum Piece { EMPTY = 0, P = 1, N = 2, B = 3, R = 4, Q = 5, K = 6 };

#include <vector>

struct GameState {
    int board[128];
    int sideToMove;
    int enPassantSquare;
    bool castleWK, castleWQ, castleBK, castleBQ;
};

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
    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;
    const int SIDEBAR_X = 700;

    Font uiFont;

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

    // Game history trackers
    std::vector<GameState> undoStack;
    std::vector<GameState> redoStack;

    std::vector<std::string> moveHistory;
    std::vector<std::string> redoMoveHistory;

    std::string GetUCIMove(Move m);

    ChessGame();
    ~ChessGame();

    void SaveStateTo(std::vector<GameState>& stack);
    void Undo();
    void Redo();

    // In game methods

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

    // Post game methods
    void CheckForGameOver();
    void DrawGameOver();

    // Performance and Logic Testing
    long long Perft(int depth);
    void RunPerftTest(int depth);

    // AI: Minimax
    int Minimax(int depth, bool isMaximizingPlayer);
    Move FindBestMove(int depth);

    // FEN Parser
    void LoadFromFEN(const std::string& fen);

    // UI handling
    void DrawSidebar();
    void DrawCoordinates();
};

#endif
