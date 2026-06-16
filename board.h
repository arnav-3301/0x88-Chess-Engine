#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "raylib.h"

enum Piece { EMPTY = 0, P = 1, N = 2, B = 3, R = 4, Q = 5, K = 6 };

#include <vector>

struct CastleFlags {
    bool WK;
    bool WQ;
    bool BK;
    bool BQ;
    CastleFlags(bool f): WK(f), WQ(f), BK(f), BQ(f) {};
};

struct GameState {
    int board[128];
    int sideToMove;
    int enPassantSquare;
    CastleFlags castleFlags{false};
    int kingSquareWhite;
    int kingSquareBlack;
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
    // constants
    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;
    const int SIDEBAR_X = 700;

    // 0x88 board
    int board[128];

    // square variables
    int selectedSquare;
    int enPassantSquare;
    int kingSquareWhite;
    int kingSquareBlack;

    // turn of player, white(1), black(-1)
    int sideToMove;

    // Bot settings
    int botSearchDepth;

    // Game flags
    CastleFlags castleFlags{true};
    bool isPromoting;
    bool isCheckmate;
    bool isStalemate;

    // Mode flags
    bool isAutoPlay;
    bool isHeadlessMode;

    // ??
    Move pendingPromotionMove;

    // Move Cache
    std::vector<Move> activeMoves;

    // Constructor and Destructor
    ChessGame(bool headless = false);
    ~ChessGame();

    // Board init
    void InitStartingPosition();

    // Game history trackers
    std::vector<GameState> undoStack;
    std::vector<GameState> redoStack;

    std::vector<std::string> moveHistory;
    std::vector<std::string> redoMoveHistory;

    void SaveStateTo(std::vector<GameState>& stack);
    void Undo();
    void Redo();

    void SaveState(GameState& state);
    void RestoreState(const GameState& state);


    // In game methods
    bool isSquareOnBoard(int index);
    bool isSquareAttacked(int square, int attackingColorSign);
    bool IsRepetition();


    // Move generation
    std::vector<Move> GeneratePseudoLegalMoves(int colorToGenerate);
    std::vector<Move> GenerateLegalMoves(int colorToGenerate);

    // In game methods and move updates
    void MakeMove(Move m);
    void Update();
    void AutoPlayUpdate();
    void CheckForGameOver();


    // Engine Correctness Testing
    long long Perft(int depth);
    void RunPerftTest(int depth);

    // Game Engine
    int Evaluate();
    int Minimax(int depth, int alpha, int beta, bool isMaximizingPlayer);
    Move FindBestMove(int depth);

    // Redundant/Obsolete gonna remove later
    int FindKingWhite();
    int FindKingBlack();

    // FEN Parser
    void LoadFromFEN(const std::string& fen);
    std::string GetUCIMove(Move m);

    // UI handling
    Font uiFont;
    Texture2D whiteTextures[7];
    Texture2D blackTextures[7];

    void LoadPieceTextures();
    void UnloadPieceTextures();
    void DrawBoard();
    void DrawPieces();
    void DrawPromotionMenu();
    void DrawMoves();
    void DrawGameOver();
    void DrawSidebar();
    void DrawCoordinates();
};



#endif
