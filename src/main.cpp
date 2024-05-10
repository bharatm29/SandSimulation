#include <functional>
#include <iostream>
#include <random>
#include <raylib.h>
#include <time.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 10
#define CELL_HEIGHT CELL_WIDTH

#define COLS WIDTH / CELL_WIDTH
#define ROWS HEIGHT / CELL_HEIGHT

float gen_random_float(float min, float max) {
    typedef std::mt19937 Engine;
    typedef std::uniform_real_distribution<float> Distribution;

    auto r =
        std::bind(Distribution(0.0f, 1.0f), Engine((unsigned int)time(NULL)));
    return r();
}

int calculateAliveNeighbors(const int r, const int c, vector<vector<int>> &v) {
    int count{0};

    vector<std::pair<int, int>> dirs = {{0, -1},  {0, 1},  {-1, 0}, {1, 0},
                                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (const std::pair dir : dirs) {
        const int nr = r + dir.first;
        const int nc = c + dir.second;

        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
            if (v[nr][nc] == 1) {
                count++;
            }
        }
    }

    return count;
}

void updateState(vector<vector<int>> &v) {
    vector<vector<int>> vCopy = v;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            const int aliveNeighbours = calculateAliveNeighbors(r, c, vCopy);
            if (v[r][c] == 0) {
                if (aliveNeighbours == 3) {
                    v[r][c] = 1;
                }
            } else {
                if (aliveNeighbours < 2) {
                    v[r][c] = 0;
                } else if (aliveNeighbours == 2 || aliveNeighbours == 3) {
                    v[r][c] = 1;
                } else if (aliveNeighbours > 3) {
                    v[r][c] = 0;
                }
            }
        }
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(60);

    bool eraseMode = false;
    bool animateMode = false;
    unsigned int frameCounter = 0;

    vector<vector<int>> v(ROWS, vector<int>(COLS, 0));

    while (!WindowShouldClose()) {
        BeginDrawing();

        {
            ClearBackground(BLACK);

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                eraseMode = true;
            } else {
                eraseMode = false;
            }

            if (IsKeyPressed(KEY_SPACE)) {
                animateMode = !animateMode;
            }

            if (animateMode) {
                frameCounter++;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX() / CELL_WIDTH;
                const int celly = GetMouseY() / CELL_HEIGHT;

                v[celly][cellx] = eraseMode ? 0 : 1;
            }

            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (v[r][c] == 1) {
                        Vector2 position = Vector2{.x = (float)CELL_WIDTH * c,
                                                   .y = (float)CELL_HEIGHT * r};

                        Vector2 size =
                            Vector2{.x = CELL_WIDTH, .y = CELL_HEIGHT};

                        DrawRectangleV(position, size, RED);

                        // FIXME: DrawRectangleLines(0, 0, CELL_WIDTH,
                        // CELL_HEIGHT, RED);
                    }
                }
            }

            if (animateMode && frameCounter == 30) {
                updateState(v);
                frameCounter = 0;
            } else if (!animateMode && IsKeyPressed(KEY_ENTER)) {
                updateState(v);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
