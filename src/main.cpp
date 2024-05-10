#include <functional>
#include <iostream>
#include <random>
#include <raylib.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 10
#define CELL_HEIGHT CELL_WIDTH

float gen_random_float(float min, float max) {
    typedef std::mt19937 Engine;
    typedef std::uniform_real_distribution<float> Distribution;

    auto r =
        std::bind(Distribution(0.0f, 1.0f), Engine((unsigned int)time(NULL)));
    return r();
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(60);

    const int rows = HEIGHT / CELL_HEIGHT;
    const int cols = WIDTH / CELL_WIDTH;

    bool eraseMode = false;

    vector<vector<int>> v(rows, vector<int>(cols, 0));

    while (!WindowShouldClose()) {
        BeginDrawing();

        {
            ClearBackground(BLACK);
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                eraseMode = true;
            } else {
                eraseMode = false;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX() / CELL_WIDTH;
                const int celly = GetMouseY() / CELL_HEIGHT;

                v[celly][cellx] = eraseMode ? 0 : 1;
            }

            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
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
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
