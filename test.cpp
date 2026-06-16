#include <iostream>
#include <string>
#include <chrono>
#include "board.h" // Assuming this contains your ChessGame class definition

int main(int argc, char* argv[]) {
    // 1. Ensure the user provided a depth argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <depth>\n";
        std::cerr << "Example: " << argv[0] << " 5\n";
        return 1;
    }

    // 2. Parse the target depth securely
    int depth = 0;
    try {
        depth = std::stoi(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Depth must be a valid integer.\n";
        return 1;
    }

    if (depth < 1) {
        std::cerr << "Error: Depth must be greater than 0.\n";
        return 1;
    }

    std::cout << "Initializing Engine in Headless Mode...\n";
    std::cout << "Target Depth: " << depth << "\n";
    
    // 3. Instantiate the game using your headless constructor
    ChessGame game(true);

    // 4. Start the precision timer
    auto startTime = std::chrono::high_resolution_clock::now();

    // 5. Run the Divide / Perft Test
    game.RunPerftTest(depth);

    // 6. Stop the timer and calculate the duration
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Print final benchmark stats
    std::cout << "Time elapsed: " << duration.count() << " ms\n";

    return 0;
}
