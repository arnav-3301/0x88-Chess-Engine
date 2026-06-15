#include "board.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <sstream>
#include <cctype>

const int knightOffsets[8] = {-33, -31, -18, -14, 14, 18, 31, 33};
const int rookOffsets[4]   = {-16, 16, -1, 1};
const int bishopOffsets[4] = {-17, -15, 15, 17};
const int queenOffsets[8]  = {-17, -16, -15, -1, 1, 15, 16, 17};


// Move to notation (UCI)
std::string ChessGame::GetUCIMove(Move m) {
    // Lambda helper to convert a single 0x88 index to a 2-character string
    auto indexToAlgebraic = [](int index) {
        int file = index & 7; // Get the column (0-7)
        int rank = 8 - (index >> 4); // Map visual Raylib rank to chess rank (1-8)

        char fileChar = 'a' + file;
        return std::string(1, fileChar) + std::to_string(rank);
    };

    std::string moveStr = indexToAlgebraic(m.from) + indexToAlgebraic(m.to);

    // Append promotion piece if applicable
    if (m.promotionPiece != 0) {
        switch(std::abs(m.promotionPiece)) {
            case Q: moveStr += "q"; break;
            case R: moveStr += "r"; break;
            case B: moveStr += "b"; break;
            case N: moveStr += "n"; break;
        }
    }

    return moveStr;
}

// Game history methods
void ChessGame::SaveStateTo(std::vector<GameState>& stack) {
    GameState state;
    std::copy(std::begin(board), std::end(board), std::begin(state.board));
    state.sideToMove = sideToMove;
    state.enPassantSquare = enPassantSquare;
    state.castleWK = castleWK;
    state.castleWQ = castleWQ;
    state.castleBK = castleBK;
    state.castleBQ = castleBQ;

    stack.push_back(state);
}
void ChessGame::Undo() {
    if (undoStack.empty()) return;

    redoMoveHistory.push_back(moveHistory.back());
    moveHistory.pop_back();

    // 1. Save current state to the redo timeline
    SaveStateTo(redoStack);

    // 2. Fetch the previous state
    GameState prevState = undoStack.back();
    undoStack.pop_back();

    // 3. Restore the board
    std::copy(std::begin(prevState.board), std::end(prevState.board), std::begin(board));
    sideToMove = prevState.sideToMove;
    enPassantSquare = prevState.enPassantSquare;
    castleWK = prevState.castleWK;
    castleWQ = prevState.castleWQ;
    castleBK = prevState.castleBK;
    castleBQ = prevState.castleBQ;

    // 4. Reset UI and locks
    selectedSquare = -1;
    activeMoves.clear();
    isCheckmate = false;
    isStalemate = false;
    isPromoting = false;
}

void ChessGame::Redo() {
    if (redoStack.empty()) return;

    moveHistory.push_back(redoMoveHistory.back());
    redoMoveHistory.pop_back();

    // 1. Save current state back to the undo timeline
    SaveStateTo(undoStack);

    // 2. Fetch the next state
    GameState nextState = redoStack.back();
    redoStack.pop_back();

    // 3. Restore the board
    std::copy(std::begin(nextState.board), std::end(nextState.board), std::begin(board));
    sideToMove = nextState.sideToMove;
    enPassantSquare = nextState.enPassantSquare;
    castleWK = nextState.castleWK;
    castleWQ = nextState.castleWQ;
    castleBK = nextState.castleBK;
    castleBQ = nextState.castleBQ;

    // 4. Reset UI and evaluate game over conditions for the restored turn
    selectedSquare = -1;
    activeMoves.clear();
    CheckForGameOver(); 
}

// FEN Parser
void ChessGame::LoadFromFEN(const std::string& fen) {
    // 1. Wipe the board clean
    for (int i = 0; i < 128; i++) board[i] = EMPTY;

    std::istringstream ss(fen);
    std::string pieces, color, castling, enPassant;
    
    // We only need the first 4 parts for our engine's logic
    ss >> pieces >> color >> castling >> enPassant; 

    // 2. Parse Piece Placement
    int file = 0;
    int rank = 0; // Rank 0 is the top of your screen (Black's 8th rank)

    for (char c : pieces) {
        if (c == '/') {
            rank++;
            file = 0;
        } else if (isdigit(c)) {
            file += (c - '0'); // If it's a number (e.g., '3'), skip 3 empty squares
        } else {
            int square = (rank << 4) + file;
            switch (c) {
                case 'P': board[square] = P; break;
                case 'N': board[square] = N; break;
                case 'B': board[square] = B; break;
                case 'R': board[square] = R; break;
                case 'Q': board[square] = Q; break;
                case 'K': board[square] = K; break;
                case 'p': board[square] = -P; break;
                case 'n': board[square] = -N; break;
                case 'b': board[square] = -B; break;
                case 'r': board[square] = -R; break;
                case 'q': board[square] = -Q; break;
                case 'k': board[square] = -K; break;
            }
            file++;
        }
    }

    // 3. Parse Active Color
    sideToMove = (color == "w") ? 1 : -1;

    // 4. Parse Castling Rights
    castleWK = false; castleWQ = false; castleBK = false; castleBQ = false;
    for (char c : castling) {
        if (c == 'K') castleWK = true;
        if (c == 'Q') castleWQ = true;
        if (c == 'k') castleBK = true;
        if (c == 'q') castleBQ = true;
    }

    // 5. Parse En Passant Target Square
    if (enPassant == "-") {
        enPassantSquare = -1;
    } else {
        int fileIdx = enPassant[0] - 'a';
        int rankChar = enPassant[1] - '0';
        // Map standard chess rank (1-8) to our visual Raylib rank (7-0)
        int rankIdx = 8 - rankChar; 
        enPassantSquare = (rankIdx << 4) + fileIdx;
    }
}

ChessGame::ChessGame() {
    InitStartingPosition();
    LoadPieceTextures();
    uiFont = LoadFontEx("myFont.ttf", 60, 0, 250);

    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    selectedSquare = -1;
    enPassantSquare = -1;
    sideToMove = 1;
    castleWK = true;
    castleWQ = true;
    castleBK = true;
    castleBQ = true;

    isPromoting = false;
    isCheckmate = false;
    isStalemate = false;
}

ChessGame::~ChessGame() {
    UnloadPieceTextures();
    UnloadFont(uiFont);
}

bool ChessGame::isSquareOnBoard(int index) {
    return (index & 0x88) == 0;
}

bool ChessGame::isSquareAttacked(int square, int attackingColorSign) {

    for (int i = 0; i < 8; i++) {
        int targetSquare = square + knightOffsets[i];
        if (isSquareOnBoard(targetSquare)) {
            int piece = board[targetSquare];
            if (piece != EMPTY && std::abs(piece) == N && (piece * attackingColorSign) > 0) return true;
        }
    }


    for (int i = 0; i < 8; i++) {
        int targetSquare = square + queenOffsets[i];
        if (isSquareOnBoard(targetSquare)) {
            int piece = board[targetSquare];
            if (piece != EMPTY && std::abs(piece) == K && (piece * attackingColorSign) > 0) return true;
        }
    }


    int pawnOffset = 16 * attackingColorSign; 
    int pawnAttacks[2] = {pawnOffset - 1, pawnOffset + 1};
    for (int i = 0; i < 2; i++) {
        int targetSquare = square + pawnAttacks[i];
        if (isSquareOnBoard(targetSquare)) {
            int piece = board[targetSquare];
            if (piece != EMPTY && std::abs(piece) == P && (piece * attackingColorSign) > 0) return true;
        }
    }


    for (int i = 0; i < 4; i++) {
        int targetSquare = square + bishopOffsets[i];
        while (isSquareOnBoard(targetSquare)) {
            int piece = board[targetSquare];
            if (piece != EMPTY) {
                if ((std::abs(piece) == B || std::abs(piece) == Q) && (piece * attackingColorSign) > 0) return true;
                break;
            }
            targetSquare += bishopOffsets[i];
        }
    }


    for (int i = 0; i < 4; i++) {
        int targetSquare = square + rookOffsets[i];
        while (isSquareOnBoard(targetSquare)) {
            int piece = board[targetSquare];
            if (piece != EMPTY) {
                if ((std::abs(piece) == R || std::abs(piece) == Q) && (piece * attackingColorSign) > 0) return true;
                break; 
            }
            targetSquare += rookOffsets[i];
        }
    }

    return false;
}

void ChessGame::InitStartingPosition() {
    // 1. Wipe the board clean
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }

    // 2. Set up Black Pieces (Top of screen, indices 0-7 and 16-23)
    board[0] = -R; board[1] = -N; board[2] = -B; board[3] = -Q;
    board[4] = -K; board[5] = -B; board[6] = -N; board[7] = -R;
    for (int i = 16; i < 24; i++) board[i] = -P;

    // 3. Set up White Pieces (Bottom of screen, indices 112-119 and 96-103)
    board[112] = R; board[113] = N; board[114] = B; board[115] = Q;
    board[116] = K; board[117] = B; board[118] = N; board[119] = R;
    for (int i = 96; i < 104; i++) board[i] = P;

    // 4. Reset Game State
    sideToMove = 1;
    enPassantSquare = -1;
    castleWK = true; 
    castleWQ = true;
    castleBK = true; 
    castleBQ = true;
}
void ChessGame::LoadPieceTextures() {
    whiteTextures[P] = LoadTexture("resources/white-pawn.png");
    whiteTextures[N] = LoadTexture("resources/white-knight.png");
    whiteTextures[B] = LoadTexture("resources/white-bishop.png");
    whiteTextures[R] = LoadTexture("resources/white-rook.png");
    whiteTextures[Q] = LoadTexture("resources/white-queen.png");
    whiteTextures[K] = LoadTexture("resources/white-king.png");

    blackTextures[P] = LoadTexture("resources/black-pawn.png");
    blackTextures[N] = LoadTexture("resources/black-knight.png");
    blackTextures[B] = LoadTexture("resources/black-bishop.png");
    blackTextures[R] = LoadTexture("resources/black-rook.png");
    blackTextures[Q] = LoadTexture("resources/black-queen.png");
    blackTextures[K] = LoadTexture("resources/black-king.png");
}

void ChessGame::UnloadPieceTextures() {
    for (int i = 1; i <= 6; i++) {
        UnloadTexture(whiteTextures[i]);
        UnloadTexture(blackTextures[i]);
    }
}

void ChessGame::DrawBoard() {
    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isLightSquare = (rank + file) % 2 == 0;
            Color squareColor = isLightSquare ? RAYWHITE : GRAY;
            int screenX = file * SQUARE_SIZE + BOARD_OFFSET_X;
            int screenY = rank * SQUARE_SIZE + BOARD_OFFSET_Y;
            DrawRectangle(screenX, screenY, SQUARE_SIZE, SQUARE_SIZE, squareColor);
        }
    }
}

void ChessGame::DrawPieces() {
    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int index = (rank << 4) + file;
            int piece = board[index];

            if (piece != EMPTY) {
                int pieceType = std::abs(piece);
                Texture2D currentTexture = (piece > 0) ? whiteTextures[pieceType] : blackTextures[pieceType];

                float posX = file * SQUARE_SIZE + BOARD_OFFSET_X;
                float posY = rank * SQUARE_SIZE + BOARD_OFFSET_Y;

                Vector2 position = { posX, posY };

                float scale = (float)SQUARE_SIZE / currentTexture.width;
                DrawTextureEx(currentTexture, position, 0.0f, scale, WHITE);
            }
        }
    }
}

void ChessGame::Update() {
    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;


    if (IsKeyPressed(KEY_LEFT)) Undo();
    if (IsKeyPressed(KEY_RIGHT)) Redo();
    if (isCheckmate || isStalemate) return;

    if(isPromoting) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            int boardX = mouseX - BOARD_OFFSET_X;
            int boardY = mouseY - BOARD_OFFSET_Y;

            int clickedFile = boardX / SQUARE_SIZE;
            int clickedRank = boardY / SQUARE_SIZE;

            int targetRank = pendingPromotionMove.to >> 4;
            int targetFile = pendingPromotionMove.to & 7;


            if (clickedFile == targetFile) {
                int direction = (targetRank == 0) ? 1 : -1;

                for (int i = 0; i < 4; i++) {
                    int menuRank = targetRank + (i * direction);
                    if (clickedRank == menuRank) {

                        int promoPieces[4] = {Q, R, B, N};
                        pendingPromotionMove.promotionPiece = promoPieces[i];

                        SaveStateTo(undoStack);
                        redoStack.clear();

                        moveHistory.push_back(GetUCIMove(pendingPromotionMove));
                        redoMoveHistory.clear();

                        MakeMove(pendingPromotionMove);

                        isPromoting = false;
                        selectedSquare = -1;
                        activeMoves.clear();

                        CheckForGameOver();
                        return;
                    }
                }
            }
        }
        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        int boardX = mouseX - BOARD_OFFSET_X;
        int boardY = mouseY - BOARD_OFFSET_Y;

            int file = boardX / SQUARE_SIZE;
            int rank = boardY / SQUARE_SIZE;
            int clickedIndex = (rank << 4) + file;


        if (mouseX >= SIDEBAR_X) {

        }
        if (!isSquareOnBoard(clickedIndex)) return;

        if (selectedSquare != -1) {
            bool moveExecuted = false;

            for (const Move& m : activeMoves) {
                if (m.from == selectedSquare && m.to == clickedIndex) {

                    if (m.promotionPiece != 0) {
                        isPromoting = true;
                        pendingPromotionMove = m;
                        moveExecuted = true;
                        break;
                    }

                    SaveStateTo(undoStack);
                    redoStack.clear();

                    moveHistory.push_back(GetUCIMove(m));
                    redoMoveHistory.clear();

                    MakeMove(m);
                    selectedSquare = -1;
                    activeMoves.clear();
                    moveExecuted = true;

                    CheckForGameOver();
                    break;
                }
            }

            if (moveExecuted) return;
        }
        int clickedPiece = board[clickedIndex];

        if (clickedPiece != EMPTY && (clickedPiece * sideToMove) > 0) {
            selectedSquare = clickedIndex;
            activeMoves.clear();

            std::vector<Move> allLegalMoves = GenerateLegalMoves(sideToMove);


            for (const Move& m : allLegalMoves) {
                if (m.from == selectedSquare) {
                    activeMoves.push_back(m);
                }
            }

        } else {
            selectedSquare = -1;
            activeMoves.clear();
        }
    }
}
void ChessGame::MakeMove(Move m) {
    int piece = board[m.from];
    int activeColorSign = (piece > 0) ? 1 : -1;

    if (m.isEnPassant) {
        int forwardOffset = -16 * activeColorSign;
        int capPawnSquare = m.to - forwardOffset;
        board[capPawnSquare] = EMPTY;
    }

    enPassantSquare = -1;

    if (std::abs(piece) == P && std::abs(m.to - m.from) == 32) {
        enPassantSquare = (m.from + m.to) / 2;
    }
    // Revoking castling rights
    if (piece == K)  { castleWK = false; castleWQ = false; }
    if (piece == -K) { castleBK = false; castleBQ = false; }


    if (m.from == 0   || m.to == 0)   castleWQ = false;
    if (m.from == 7   || m.to == 7)   castleWK = false;
    if (m.from == 112 || m.to == 112) castleBQ = false;
    if (m.from == 119 || m.to == 119) castleBK = false;

    if (m.isCastling) {
        if (m.to == 6) {
            board[5] = board[7]; board[7] = EMPTY;
        } else if (m.to == 2) {
            board[3] = board[0]; board[0] = EMPTY;
        } else if (m.to == 118) {
            board[117] = board[119]; board[119] = EMPTY;
        } else if (m.to == 114) {
            board[115] = board[112]; board[112] = EMPTY;
        }
    }

    board[m.to] = piece;
    board[m.from] = EMPTY;

    if (m.promotionPiece != 0) {
        board[m.to] = m.promotionPiece * activeColorSign;
    }

    sideToMove = -sideToMove;
}

void ChessGame::DrawMoves() {
    if (selectedSquare == -1) return;

    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;

    for (const Move& m : activeMoves) {
        if (m.from == selectedSquare) {
            int targetRank = m.to >> 4;
            int targetFile = m.to & 7;

            int centerX = (targetFile * SQUARE_SIZE + BOARD_OFFSET_X) + (SQUARE_SIZE / 2);
            int centerY = (targetRank * SQUARE_SIZE + BOARD_OFFSET_Y) + (SQUARE_SIZE / 2);

            DrawCircle(centerX, centerY, 15, Fade(DARKGREEN, 0.7f));
        }
    }
}


void ChessGame::DrawPromotionMenu() {
    if (!isPromoting) return;

    const int SQUARE_SIZE = 80;
    const int BOARD_OFFSET_X = 40;
    const int BOARD_OFFSET_Y = 30;

    DrawRectangle(BOARD_OFFSET_X, BOARD_OFFSET_Y, 8 * SQUARE_SIZE, 8 * SQUARE_SIZE, Fade(BLACK, 0.5f));

    int targetRank = pendingPromotionMove.to >> 4;
    int targetFile = pendingPromotionMove.to & 7;
    int direction = (targetRank == 0) ? 1 : -1;

    int promoPieces[4] = {Q, R, B, N};

    Texture2D* textures = (sideToMove == 1) ? whiteTextures : blackTextures;

    for (int i = 0; i < 4; i++) {
        int menuRank = targetRank + (i * direction);
        int drawX = targetFile * SQUARE_SIZE + BOARD_OFFSET_X;
        int drawY = menuRank * SQUARE_SIZE + BOARD_OFFSET_Y;

        DrawRectangle(drawX, drawY, SQUARE_SIZE, SQUARE_SIZE, RAYWHITE);
        DrawRectangleLines(drawX, drawY, SQUARE_SIZE, SQUARE_SIZE, BLACK);

        int pieceToDraw = promoPieces[i];
        Texture2D texture = textures[pieceToDraw];

        float scale = (float)SQUARE_SIZE / texture.width;
        DrawTextureEx(texture, {(float)drawX, (float)drawY}, 0.0f, scale, WHITE);
    }
}

void ChessGame::CheckForGameOver() {
    std::vector<Move> currentMoves = GenerateLegalMoves(sideToMove);

    if (!currentMoves.empty()) return;

    int kingSquare = -1;
    int kingPiece = (sideToMove == 1) ? K : -K;

    for (int i = 0; i < 128; i++) {
        if (isSquareOnBoard(i) && board[i] == kingPiece) {
            kingSquare = i;
            break;
        }
    }

    if (isSquareAttacked(kingSquare, -sideToMove)) {
        isCheckmate = true;
    } else {
        isStalemate = true;
    }
}

void ChessGame::DrawGameOver() {
    if (!isCheckmate && !isStalemate) return;

    const float SQUARE_SIZE = 80.0f;
    const float BOARD_OFFSET_X = 40.0f;
    const float BOARD_OFFSET_Y = 30.0f;

    float screenWidth = 8 * SQUARE_SIZE;
    float screenHeight = 8 * SQUARE_SIZE;

    // Dim the entire board
    DrawRectangle(BOARD_OFFSET_X, BOARD_OFFSET_Y, screenWidth, screenHeight, Fade(BLACK, 0.7f));

    if (isCheckmate) {
        const char* text = (sideToMove == 1) ? "BLACK WINS!" : "WHITE WINS!";

        // --- NEW: Measure custom font width using MeasureTextEx().x ---
        Vector2 textSize = MeasureTextEx(uiFont, text, 60.0f, 1.0f);
        Vector2 headerPos = { 
            (float)((screenWidth - textSize.x) / 2 + BOARD_OFFSET_X), 
            (float)(screenHeight / 2 - 40.0f + BOARD_OFFSET_Y)
        };
        DrawTextEx(uiFont, text, headerPos, 60.0f, 1.0f, RED);

        Vector2 subTextSize = MeasureTextEx(uiFont, "CHECKMATE", 30.0f, 1.0f);
        Vector2 subHeaderPos = {
            (float)((screenWidth - subTextSize.x) / 2 + BOARD_OFFSET_X), 
            (float)(screenHeight / 2 + 30.0f + BOARD_OFFSET_Y)
        };
        DrawTextEx(uiFont, "CHECKMATE", subHeaderPos, 30.0f, 1.0f, WHITE);
    } 
    else if (isStalemate) {
        Vector2 textSize = MeasureTextEx(uiFont, "STALEMATE", 60.0f, 1.0f);
        Vector2 headerPos = { 
            (float)((screenWidth - textSize.x) / 2 + BOARD_OFFSET_X), 
            (float)(screenHeight / 2 - 40.0f + BOARD_OFFSET_Y)
        };
        DrawTextEx(uiFont, "STALEMATE", headerPos, 60.0f, 1.0f, GRAY);

        Vector2 subTextSize = MeasureTextEx(uiFont, "DRAW", 30.0f, 1.0f);
        Vector2 subHeaderPos = {
            (float)((screenWidth - subTextSize.x) / 2 + BOARD_OFFSET_X), 
            (float)(screenHeight / 2 + 30.0f + BOARD_OFFSET_Y)
        };
        DrawTextEx(uiFont, "DRAW", subHeaderPos, 30.0f, 1.0f, WHITE);
    }
}
// UI methods

void ChessGame::DrawCoordinates() {
    Color textColor = LIGHTGRAY;
    float fontSize = 20.0f;

    for (int i = 0; i < 8; i++) {
        int rankY = (i * SQUARE_SIZE) + (float)BOARD_OFFSET_Y + ((float)SQUARE_SIZE / 2) - (fontSize / 2);
        const char* rankText = TextFormat("%d", 8 - i);

        // Draw it 25 pixels to the left of the board edge
        Vector2 rankPos = {(float)(BOARD_OFFSET_X - 25), (float)(rankY)};
        DrawTextEx(uiFont, rankText, rankPos, fontSize, 1.0f, textColor);

        // --- Draw Files (a to h) on the bottom ---
        // Center the text horizontally relative to each square
        int fileX = (i * SQUARE_SIZE) + (float)BOARD_OFFSET_X + ((float)SQUARE_SIZE / 2) - (fontSize / 4);
        char fileChar = 'a' + i;
        const char* fileText = TextFormat("%c", fileChar);

        // Draw it 10 pixels below the bottom board edge
        Vector2 filePos = {(float)(fileX), (float)(BOARD_OFFSET_Y + (8 * SQUARE_SIZE))};
        DrawTextEx(uiFont, fileText, filePos, fontSize, 1.0f, textColor);
    }
}

void ChessGame::DrawSidebar() {
    int sidebarWidth = 1000 - SIDEBAR_X;
    DrawRectangle(SIDEBAR_X, 0, sidebarWidth, 700, GetColor(0x242424FF)); 
    DrawLine(SIDEBAR_X, 0, SIDEBAR_X, 700, DARKGRAY);

    Vector2 headerPos = { (float)(SIDEBAR_X + 20), 30.0f };
    DrawTextEx(uiFont, "ENGINE CONTROL", headerPos, 30.0f, 1.0f, LIGHTGRAY); // Changed to 1.0f
    DrawLine(SIDEBAR_X + 20, 60, SIDEBAR_X + sidebarWidth - 20, 60, DARKGRAY);

    Vector2 subHeaderPos = {(float)(SIDEBAR_X + 20), 90.0f };
    DrawTextEx(uiFont, "MOVE HISTORY", subHeaderPos, 30.0f, 1.0f, LIGHTGRAY); // Changed to 1.0f
    DrawLine(SIDEBAR_X + 20, 120, SIDEBAR_X + sidebarWidth - 20, 120, DARKGRAY);

    int startY = 140;
    int currentX = SIDEBAR_X + 20;
    int currentY = startY;

    for (size_t i = 0; i < moveHistory.size(); i++) {
        // If 'i' is even, it's White's move. If odd, it's Black's move.
        if (i % 2 == 0) {
            // Draw Turn Number (1., 2., 3.)
            Vector2 numPos = {(float)(currentX), (float)(currentY)};
            DrawTextEx(uiFont, TextFormat("%d.", (i/2) + 1), numPos, 20.0f, 0.0f, GRAY);

            // Draw White's Move
            Vector2 wMovePos = {(float)(currentX + 40), (float)(currentY)};
            DrawTextEx(uiFont, moveHistory[i].c_str(), wMovePos, 20.0f, 1.0f, WHITE);
        } else {
            // Draw Black's Move
            Vector2 bMovePos = {(float)(currentX + 130), (float)(currentY)};
            DrawTextEx(uiFont, moveHistory[i].c_str(), bMovePos, 20.0f, 1.0f, LIGHTGRAY);

            // Move down to the next line for the next turn
            currentY += 30; 
        }

        // Failsafe: Prevent the text from drawing off the bottom of the screen
        if (currentY > 660) break; 
    }
}
