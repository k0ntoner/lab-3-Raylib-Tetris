#include "raylib.h"
#include <iostream>
#include <ctime>
#include <vector>
using namespace std;
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 800
#define BLOCK_SIZE    40
#define GRID_WIDTH    10
#define GRID_HEIGHT   20

int grid[GRID_WIDTH][GRID_HEIGHT] = { 0 };
float timeSinceLastFall = 0.0f;
const float fallInterval = 0.5f;
using ShapeType = vector<vector<int>>;
struct Tetromino {
    ShapeType shape;
    Color color;
    int posX, posY;
} currentTetromino;

void InitTetromino(Tetromino* tetromino) {
    tetromino->posX = GRID_WIDTH / 2 - 1;
    tetromino->posY = 0;
    std::srand(std::time(0));
    int randomShape = std::rand() % 7;
    // Example of "I" shape
    vector<ShapeType> shapes = {
        // I-фигура
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // O-фигура
        {
            {1, 1, 0, 0},
            {1, 1, 0,0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // T-фигура
        {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // L-фигура
        {
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // J-фигура
        {
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // S-фигура
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Z-фигура
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }
    };
    tetromino->shape= shapes[randomShape];
    tetromino->color = RED;
}

bool IsValidMove(const Tetromino* tetromino, int offsetX, int offsetY, const ShapeType& newShape) {
    for (size_t i = 0; i < newShape.size(); ++i) {
        for (size_t j = 0; j < newShape[i].size(); ++j) {
            if (newShape[i][j] != 0) {
                int x = tetromino->posX + j + offsetX;
                int y = tetromino->posY + i + offsetY;

                if (x < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT || (y >= 0 && grid[x][y] != 0)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void PlaceTetromino(const Tetromino* tetromino) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (tetromino->shape[i][j] != 0) {

                int x = tetromino->posX + j;
                int y = tetromino->posY + i;
                if (y >= 0) {
                    grid[x][y] = 1;
                }
            }
        }
    }
}

void DrawTetromino(const Tetromino* tetromino) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (tetromino->shape[i][j] != 0) {
                int x = (tetromino->posX + j) * BLOCK_SIZE;
                int y = (tetromino->posY + i) * BLOCK_SIZE;

                DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, tetromino->color);
                DrawRectangleLines(x, y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
            }
        }
    }
}

void DrawGrid() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            if (grid[i][j] != 0) {
                int x = i * BLOCK_SIZE;
                int y = j * BLOCK_SIZE;
                DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, GREEN);
                DrawRectangleLines(x, y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
            }
        }
    }
}
void full_blocks() {
  
        for (int i = GRID_HEIGHT - 1; i > 0; i--) {
            int k = 0;
            for (int j = 0; j < GRID_WIDTH; j++) {
                if (grid[j][i] == 1) {
                    k++;
                }
            }
            if (k == GRID_WIDTH) {
                // Удаление текущей линии
                for (int j = 0; j < GRID_WIDTH; j++) {
                    grid[j][i] = 0;
                }

                // Смещение блоков выше вниз
                for (int h = i - 1; h >= 0; h--) {
                    for (int j = 0; j < GRID_WIDTH; j++) {
                        grid[j][h + 1] = grid[j][h];
                    }
                }
                i++;  // Повторная проверка текущей строки после смещения
            }
        }
    
}
void rotate(Tetromino* tetromino) {
    ShapeType rotatedShape(tetromino->shape[0].size(), std::vector<int>(tetromino->shape.size(), 0));

    // Алгоритм поворота
    for (size_t i = 0; i < tetromino->shape.size(); ++i) {
        for (size_t j = 0; j < tetromino->shape[i].size(); ++j) {
            rotatedShape[j][tetromino->shape.size() - 1 - i] = tetromino->shape[i][j];
        }
    }

    // Присваивание повернутой формы текущей фигуре
   
    if (IsValidMove(tetromino, 0, 0,rotatedShape)) {
        // Если поворот возможен, присваиваем повернутую форму текущей фигуре
        tetromino->shape = rotatedShape;
    }

}
void UpdateGame() {
    full_blocks();
    timeSinceLastFall += GetFrameTime();

    if (IsKeyPressed(KEY_A)) {
        if (IsValidMove(&currentTetromino, -1, 0, currentTetromino.shape)) {
            currentTetromino.posX--;
        }
    }
    if (IsKeyPressed(KEY_D)) {
        if (IsValidMove(&currentTetromino, 1, 0, currentTetromino.shape)) {
            currentTetromino.posX++;
        }
    }
    if (IsKeyPressed(KEY_W)) {
        rotate(&currentTetromino);
    }
    if (IsKeyDown(KEY_S)) {
        if (IsValidMove(&currentTetromino, 0, 1, currentTetromino.shape)) {
            currentTetromino.posY++;
        }
    }


    if (timeSinceLastFall >= fallInterval) {
        if (IsValidMove(&currentTetromino, 0, 1, currentTetromino.shape)) {
            currentTetromino.posY++;
        }
        else {
            PlaceTetromino(&currentTetromino);
            InitTetromino(&currentTetromino);
        }

        // Сброс времени
        timeSinceLastFall = 0.0f;
    }
}


int main() {
   
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    SetTargetFPS(60);

    InitTetromino(&currentTetromino);

    while (!WindowShouldClose()) {
        UpdateGame();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawGrid();
        DrawTetromino(&currentTetromino);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}