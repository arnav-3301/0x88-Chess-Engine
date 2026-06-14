#include "board.h"
#include <iostream>
#include <algorithm>

const int knightOffsets[8] = {-33, -31, -18, -14, 14, 18, 31, 33};
const int rookOffsets[4]   = {-16, 16, -1, 1};
const int bishopOffsets[4] = {-17, -15, 15, 17};
const int queenOffsets[8]  = {-17, -16, -15, -1, 1, 15, 16, 17};


long long ChessGame::Perft(int depth) {
    if (depth == 0) return 1;

    std::vector<Move> moves = GenerateLegalMoves(sideToMove);
    long long nodes = 0;

    for (const Move& m : moves) {
        // 1. TAKE SNAPSHOT
        int tempBoard[128];
        std::copy(std::begin(board), std::end(board), std::begin(tempBoard));
        int tempEP = enPassantSquare;
        bool tempWK = castleWK;
        bool tempWQ = castleWQ;
        bool tempBK = castleBK;
        bool tempBQ = castleBQ;
        int tempSide = sideToMove;

        // 2. MAKE MOVE
        MakeMove(m);

        // 3. RECURSE DOWN THE TREE
        nodes += Perft(depth - 1);

        // 4. RESTORE SNAPSHOT
        std::copy(std::begin(tempBoard), std::end(tempBoard), std::begin(board));
        enPassantSquare = tempEP;
        castleWK = tempWK;
        castleWQ = tempWQ;
        castleBK = tempBK;
        castleBQ = tempBQ;
        sideToMove = tempSide;
    }

    return nodes;
}

void ChessGame::RunPerftTest(int depth) {
    std::cout << "\n--- Starting Perft Test to Depth " << depth << " ---\n";
    
    std::vector<Move> moves = GenerateLegalMoves(sideToMove);
    long long totalNodes = 0;

    // This is called a "Divide" - it prints the node count for each individual starting move
    for (const Move& m : moves) {
        // Snapshot
        int tempBoard[128];
        std::copy(std::begin(board), std::end(board), std::begin(tempBoard));
        int tempEP = enPassantSquare;
        bool tempWK = castleWK;
        bool tempWQ = castleWQ;
        bool tempBK = castleBK;
        bool tempBQ = castleBQ;
        int tempSide = sideToMove;

        MakeMove(m);
        long long nodes = Perft(depth - 1);
        totalNodes += nodes;

        // Restore
        std::copy(std::begin(tempBoard), std::end(tempBoard), std::begin(board));
        enPassantSquare = tempEP;
        castleWK = tempWK;
        castleWQ = tempWQ;
        castleBK = tempBK;
        castleBQ = tempBQ;
        sideToMove = tempSide;

        // Print the move format: fromIndex toIndex : nodeCount
        std::cout << "Move " << m.from << "-" << m.to << " : " << nodes << " nodes\n";
    }

    std::cout << "\nTotal Nodes Evaluated: " << totalNodes << "\n";
    std::cout << "-----------------------------------\n";
}


std::vector<Move> ChessGame::GeneratePseudoLegalMoves(int colorToGenerate) {
    std::vector<Move> moveList;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (rank << 4) + file;
            int piece = board[square];

            if (piece == EMPTY) continue;

            int pieceColorSign = (piece > 0) ? 1 : -1;

            if (pieceColorSign != colorToGenerate) continue;

            int activeColorSign = colorToGenerate;
            int pieceType = std::abs(piece);

            if (pieceType == N) {
                for (int i = 0; i < 8; i++) {
                    int targetSquare = square + knightOffsets[i];

                    if (isSquareOnBoard(targetSquare)) {
                        int targetPiece = board[targetSquare];

                        if (targetPiece == EMPTY || (targetPiece * activeColorSign) < 0) {
                            Move m;
                            m.from = square;
                            m.to = targetSquare;
                            m.isCapture = (targetPiece != EMPTY);
                            moveList.push_back(m);
                        }
                    }
                }
            }

            else if (pieceType == B || pieceType == R || pieceType == Q) {
                int numOffsets = (pieceType == Q) ? 8 : 4;
                const int* offsets = (pieceType == R) ? rookOffsets :
                                     (pieceType == B) ? bishopOffsets : queenOffsets;

                for (int i = 0; i < numOffsets; i++) {
                    int targetSquare = square + offsets[i];
                    while (isSquareOnBoard(targetSquare)) {
                        int targetPiece = board[targetSquare];
                        if (targetPiece == EMPTY) {
                            Move m; m.from = square; m.to = targetSquare;
                            m.isCapture = false;
                            moveList.push_back(m);
                        }
                        else {
                            if ((targetPiece * activeColorSign) < 0) {
                                Move m; m.from = square; m.to = targetSquare;
                                m.isCapture = true;
                                moveList.push_back(m);
                            }
                             break;
                        }

                        targetSquare += offsets[i];
                    }
                }
            }

            else if (pieceType == K) {
                for (int i = 0; i < 8; i++) {
                    int targetSquare = square + queenOffsets[i];

                    if (isSquareOnBoard(targetSquare)) {
                        int targetPiece = board[targetSquare];

                        if (targetPiece == EMPTY || (targetPiece * activeColorSign) < 0) {
                            Move m; 
                            m.from = square; 
                            m.to = targetSquare;
                            m.isCapture = (targetPiece != EMPTY);
                            moveList.push_back(m);
                        }
                    }
                }
                if (activeColorSign == 1) { 
                    if (square == 116) { 

                        if (castleWK && board[117] == EMPTY && board[118] == EMPTY) {
                            Move m; m.from = 116; m.to = 118; m.isCastling = true; m.isCapture = false;
                            moveList.push_back(m);
                        }

                        if (castleWQ && board[115] == EMPTY && board[114] == EMPTY && board[113] == EMPTY) {
                            Move m; m.from = 116; m.to = 114; m.isCastling = true; m.isCapture = false;
                            moveList.push_back(m);
                        }
                    }
                } else {
                    if (square == 4) { 

                        if (castleBK && board[5] == EMPTY && board[6] == EMPTY) {
                            Move m; m.from = 4; m.to = 6; m.isCastling = true; m.isCapture = false;
                            moveList.push_back(m);
                        }

                        if (castleBQ && board[3] == EMPTY && board[2] == EMPTY && board[1] == EMPTY) {
                            Move m; m.from = 116; m.to = 114; m.isCastling = true; m.isCapture = false;
                            moveList.push_back(m);
                        }
                    }
                }
            }

            else if (pieceType == P) {
                int forwardOffset = -16 * activeColorSign; 
                int startRank = (activeColorSign == 1) ? 6 : 1; 

                auto AddPawnMove = [&](Move m) {
                    int targetRank = m.to >> 4;

                    if ((activeColorSign == 1 && targetRank == 0) || (activeColorSign == -1 && targetRank == 7)) {
                        int promoPieces[4] = {Q, R, B, N};
                        for (int i = 0; i < 4; i++) {
                            Move promoMove = m;
                            promoMove.promotionPiece = promoPieces[i];
                            moveList.push_back(promoMove);
                        }
                    } else {
                        moveList.push_back(m);
                    }
                };

                int forwardStep = square + forwardOffset;
                if (isSquareOnBoard(forwardStep) && board[forwardStep] == EMPTY) {
                    Move m1; m1.from = square; m1.to = forwardStep; m1.isCapture = false;
                    AddPawnMove(m1); // USE THE HELPER

                    // 2. Double Push (Only if single push is valid and on start rank)
                    int currentRank = square >> 4;
                    if (currentRank == startRank) {
                        int doubleStep = square + (forwardOffset * 2);
                        if (isSquareOnBoard(doubleStep) && board[doubleStep] == EMPTY) {
                            Move m2; m2.from = square; m2.to = doubleStep; m2.isCapture = false;
                            moveList.push_back(m2); // Double push can never promote, safe to use push_back
                        }
                    }
                }

                // 3. Captures (Forward-Left and Forward-Right)
                int captureOffsets[2] = {forwardOffset - 1, forwardOffset + 1};
                for (int i = 0; i < 2; i++) {
                    int capTarget = square + captureOffsets[i];

                    if (isSquareOnBoard(capTarget)) {
                        int targetPiece = board[capTarget];

                        // Standard Capture
                        if (targetPiece != EMPTY && (targetPiece * activeColorSign) < 0) {
                            Move m3; m3.from = square; m3.to = capTarget; m3.isCapture = true;
                            AddPawnMove(m3); // USE THE HELPER
                        }
                        // En Passant Capture
                        else if (capTarget == enPassantSquare) {
                            Move m3; m3.from = square; m3.to = capTarget; m3.isCapture = true; m3.isEnPassant = true;
                            moveList.push_back(m3); // EP can never promote, safe to use push_back
                        }
                    }
                }
            }        }
    }

    return moveList;
}


std::vector<Move> ChessGame::GenerateLegalMoves(int colorToGenerate) {
    std::vector<Move> pseudoMoves = GeneratePseudoLegalMoves(colorToGenerate);
    std::vector<Move> legalMoves;

    for (const Move& m : pseudoMoves) {
        int movingPiece = board[m.from];
        int capturedPiece = board[m.to];
        int activeColorSign = (movingPiece > 0) ? 1 : -1;

        if (m.isCastling) {
            // Rule 1: Cannot castle out of check
            if (isSquareAttacked(m.from, -activeColorSign)) continue; 
            // Rule 2: Cannot castle through check
            // The transit square is exactly halfway between 'from' and 'to'
            int transitSquare = (m.from + m.to) / 2;
            if (isSquareAttacked(transitSquare, -activeColorSign)) continue;
        }

        board[m.to] = movingPiece;
        board[m.from] = EMPTY;

        int epCapturedPawnSquare = -1;
        int epPawnPiece = EMPTY;
        if (m.isEnPassant) {
            int forwardOffset = -16 * activeColorSign;
            epCapturedPawnSquare = m.to - forwardOffset;
            epPawnPiece = board[epCapturedPawnSquare];
            board[epCapturedPawnSquare] = EMPTY;
        }

        int kingSquare = -1;
        for (int i = 0; i < 128; i++) {
            if (isSquareOnBoard(i) && board[i] == K * activeColorSign) {
                kingSquare = i;
                break;
            }
        }

        if (kingSquare != -1 && !isSquareAttacked(kingSquare, -activeColorSign)) {
            legalMoves.push_back(m);
        }

        board[m.from] = movingPiece;
        board[m.to] = capturedPiece;
        if (m.isEnPassant) {
            board[epCapturedPawnSquare] = epPawnPiece; // Restore the erased pawn
        }
    }

    return legalMoves;
}
