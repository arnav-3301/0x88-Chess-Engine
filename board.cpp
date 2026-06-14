#include "board.h"
#include <cmath>

const int knightOffsets[8] = {-33, -31, -18, -14, 14, 18, 31, 33};
const int rookOffsets[4]   = {-16, 16, -1, 1};
const int bishopOffsets[4] = {-17, -15, 15, 17};
const int queenOffsets[8]  = {-17, -16, -15, -1, 1, 15, 16, 17};


ChessGame::ChessGame() {
    InitStartingPosition();
    LoadPieceTextures();
    selectedSquare = -1;
    enPassantSquare = -1;
    sideToMove = 1;
    castleWK = true;
    castleWQ = true;
    castleBK = true;
    castleBQ = true;

    isPromoting = false;
}

ChessGame::~ChessGame() {
    UnloadPieceTextures();
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
    for (int i = 0; i < 128; i++) board[i] = EMPTY;

    int startPos[64] = {
        -R, -N, -B, -Q, -K, -B, -N, -R,
        -P, -P, -P, -P, -P, -P, -P, -P,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         P,  P,  P,  P,  P,  P,  P,  P,
         R,  N,  B,  Q,  K,  B,  N,  R
        // experimental
        //  0,  0,  0,  0,  0,  0,  0,  0,
        //  P,  P,  P,  P,  P,  P,  P,  P,
        //  0,  0,  0,  0,  0,  0,  0,  0,
        // -K,  0,  0,  0,  0,  0,  0,  0,
        //  0,  0,  0,  0,  0,  0,  0,  K,
        //  0,  0,  0,  0,  0,  0,  0,  0,
        // -P, -P, -P, -P, -P, -P, -P, -P,
        //  0,  0,  0,  0,  0,  0,  0,  0
    };

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int boardIndex = (rank << 4) + file;
            board[boardIndex] = startPos[(rank * 8) + file];
        }
    }
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
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isLightSquare = (rank + file) % 2 == 0;
            Color squareColor = isLightSquare ? RAYWHITE : GRAY;
            DrawRectangle(file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, squareColor);
        }
    }
}

void ChessGame::DrawPieces() {
    const int SQUARE_SIZE = 80;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int index = (rank << 4) + file;
            int piece = board[index];

            if (piece != EMPTY) {
                int pieceType = std::abs(piece);
                Texture2D currentTexture = (piece > 0) ? whiteTextures[pieceType] : blackTextures[pieceType];

                float posX = file * SQUARE_SIZE;
                float posY = rank * SQUARE_SIZE;

                Vector2 position = { posX, posY };

                float scale = (float)SQUARE_SIZE / currentTexture.width;
                DrawTextureEx(currentTexture, position, 0.0f, scale, WHITE);
            }
        }
    }
}

void ChessGame::Update() {
    const int SQUARE_SIZE = 80;

    if(isPromoting) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            int clickedFile = mouseX / SQUARE_SIZE;
            int clickedRank = mouseY / SQUARE_SIZE;

            int targetRank = pendingPromotionMove.to >> 4;
            int targetFile = pendingPromotionMove.to & 7;


            if (clickedFile == targetFile) {
                int direction = (targetRank == 0) ? 1 : -1;

                for (int i = 0; i < 4; i++) {
                    int menuRank = targetRank + (i * direction);
                    if (clickedRank == menuRank) {

                        int promoPieces[4] = {Q, R, B, N};
                        pendingPromotionMove.promotionPiece = promoPieces[i];

                        MakeMove(pendingPromotionMove);

                        isPromoting = false;
                        selectedSquare = -1;
                        activeMoves.clear();
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

        int file = mouseX / SQUARE_SIZE;
        int rank = mouseY / SQUARE_SIZE;
        int clickedIndex = (rank << 4) + file;

        if (!isSquareOnBoard(clickedIndex)) return;

        // If a square is already selected, check if the click is on a legal move target
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

                    MakeMove(m);
                    selectedSquare = -1;
                    moveExecuted = true;
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

    for (const Move& m : activeMoves) {
        if (m.from == selectedSquare) {
            int targetRank = m.to >> 4;
            int targetFile = m.to & 7;

            int centerX = (targetFile * SQUARE_SIZE) + (SQUARE_SIZE / 2);
            int centerY = (targetRank * SQUARE_SIZE) + (SQUARE_SIZE / 2);

            DrawCircle(centerX, centerY, 15, Fade(DARKGREEN, 0.7f));
        }
    }
}


void ChessGame::DrawPromotionMenu() {
    if (!isPromoting) return;

    const int SQUARE_SIZE = 80;

    DrawRectangle(0, 0, 8 * SQUARE_SIZE, 8 * SQUARE_SIZE, Fade(BLACK, 0.5f));

    int targetRank = pendingPromotionMove.to >> 4;
    int targetFile = pendingPromotionMove.to & 7;
    int direction = (targetRank == 0) ? 1 : -1;

    int promoPieces[4] = {Q, R, B, N};

    Texture2D* textures = (sideToMove == 1) ? whiteTextures : blackTextures;

    for (int i = 0; i < 4; i++) {
        int menuRank = targetRank + (i * direction);
        int drawX = targetFile * SQUARE_SIZE;
        int drawY = menuRank * SQUARE_SIZE;

        DrawRectangle(drawX, drawY, SQUARE_SIZE, SQUARE_SIZE, RAYWHITE);
        DrawRectangleLines(drawX, drawY, SQUARE_SIZE, SQUARE_SIZE, BLACK);

        int pieceToDraw = promoPieces[i];
        Texture2D texture = textures[pieceToDraw];

        float scale = (float)SQUARE_SIZE / texture.width;
        DrawTextureEx(texture, {(float)drawX, (float)drawY}, 0.0f, scale, WHITE);
    }
}
