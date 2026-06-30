# 0x88 C++ Chess Engine

A high-performance, single-threaded C++ chess engine built on a 0x88 board representation. The project features a fully decoupled Model-View-Controller (MVC) architecture, allowing the core physics and AI to run natively in headless environments for blistering benchmark speeds (14+ Million NPS in Perft), while offering a fully interactive graphical frontend.

## Architecture & Modules

The codebase is strictly modularized to separate core game physics from rendering and AI logic.

### 1. Core Physics & State (`board.cpp` & `movegen.cpp`)
The absolute source of truth for the game state, containing zero rendering code.
* **0x88 Board Representation:** Utilizes a highly efficient 128-square 1D array to allow lightning-fast off-board detection using bitwise operations (`index & 0x88`).
* **Zobrist Hashing:** Every board state generates a unique 64-bit hash. This tracks the timeline of the game and powers instant O(1) Threefold Repetition detection, eliminating search bottlenecks in the Minimax tree.
* **Flawless Move Generation:** Pseudo-legal and strict legal move generation verified against a 3.19 Billion node Perft test (Depth 7) without missing a single en passant, castling restriction, or check-evasion edge case.

### 2. Graphical UI (`gui.cpp`)
The View and Controller module, built entirely in **Raylib**.
* **Isolated Rendering:** Reads the `ChessGame` state via reference to draw the board, textures, and move history sidebar.
* **Interactive Controller:** Handles all mouse inputs, coordinate mapping, and promotion menus, bridging the user input back to the headless physics board.

### 3. AI & Search (`bot.cpp`)
The "brain" of the engine, isolated in its own module to allow multiple instances (e.g., Bot vs. Bot arenas).
* **Minimax with Alpha-Beta Pruning:** Traverses the game tree, intelligently cutting off branches that yield worse outcomes than previously evaluated paths.
* **Transposition Table (TT):** Leverages the Zobrist Hash to cache evaluated positions. If the engine reaches an identical board state via a different move order, it instantly reads the score from memory instead of recalculating, drastically increasing search depth.
* **Move Ordering:** Optimized to search the most promising branches first, maximizing Alpha-Beta cutoffs.

## Build Targets & Testing

The project uses a structured `Makefile` that compiles completely separate binaries depending on the use case. Headless targets do not link the graphics library, ensuring pure C++ performance tests.

| Target | Description | Headless / Visual |
|---|---|---|
| `make chess_game` | Human vs. Human playable graphical interface. | Visual (Raylib) |
| `make bot_arena` | Watch two Bots of different depths fight automatically. | Visual (Raylib) |
| `make engine_trmt` | 100-game headless tournament across varied opening FENs. | Headless |
| `make perft` | Brute-force node counting to mathematically prove move logic. | Headless |
| `make search_test` | Benchmark tool to track Alpha-Beta Nodes Per Second (NPS). | Headless |
