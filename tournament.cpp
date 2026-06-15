#include "board.h"
#include <iostream>
#include <fstream>
#include <string>

void RunSingleGame(int gameNum, std::ofstream& csvFile, bool bot1IsWhite) {
    ChessGame game(true);
    game.botSearchDepth = 3; // Keep at 3 for rapid tournament processing
    
    int moveCount = 0;
    int maxMoves = 200; // Prevent infinite games if draw logic slips

    while (!game.isCheckmate && !game.isStalemate && moveCount < maxMoves) {
        // Check for 3-fold repetition inside the game loop
        if (game.IsRepetition()) {
            csvFile << gameNum << ",Draw,Repetition," << moveCount << "\n";
            std::cout << "Game " << gameNum << ": Draw (Repetition) at move " << moveCount << "\n";
            return;
        }

        Move aiMove = game.FindBestMove(game.botSearchDepth);
        
        // --- FIXED: Using the explicit SaveStateToBackup function ---
        GameState snapshot;
        game.SaveStateToBackup(snapshot);
        game.undoStack.push_back(snapshot); 
        
        game.MakeMove(aiMove);
        game.CheckForGameOver();
        moveCount++;
    }

    std::string winner = "Draw";
    std::string reason = "Max Moves";

    if (game.isCheckmate) {
        reason = "Checkmate";
        if (game.sideToMove == 1) {
            winner = bot1IsWhite ? "Bot2_Black" : "Bot1_Black";
        } else {
            winner = bot1IsWhite ? "Bot1_White" : "Bot2_White";
        }
    } else if (game.isStalemate) {
        reason = "Stalemate";
    }

    csvFile << gameNum << "," << winner << "," << reason << "," << moveCount << "\n";
    std::cout << "Game " << gameNum << ": Winner = " << winner << " (" << reason << ") in " << moveCount << " moves.\n";
}
int main() {
    std::ofstream csvFile("match_stats.csv");
    csvFile << "Game,Winner,Reason,TotalMoves\n";

    int totalGames = 1000;
    std::cout << "Starting 1000-Game Headless Engine Tournament...\n";

    for (int i = 1; i <= totalGames; i++) {
        // Alternate colors every game to ensure statistical fairness
        bool bot1IsWhite = (i % 2 != 0);
        RunSingleGame(i, csvFile, bot1IsWhite);
    }

    csvFile.close();
    std::cout << "Tournament complete. Stats saved to match_stats.csv\n";
    return 0;
}
