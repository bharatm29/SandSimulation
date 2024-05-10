#include <iostream>
#include <raylib.h>
#include <vector>

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 10
#define CELL_HEIGHT CELL_WIDTH

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(60);

    const int rows = HEIGHT / CELL_HEIGHT;
    const int cols = WIDTH / CELL_WIDTH;

    bool eraseMode = false;

    std::vector<std::vector<int>> v(rows, std::vector<int>(cols, 0));

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

                v[cellx][celly] = eraseMode ? 0 : 1;
            }

            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    if (v[r][c] == 1) {
                        Vector2 position = Vector2{.x = (float)CELL_WIDTH *
                                                        r, // why r and not c?
                                                   .y = (float)CELL_HEIGHT * c};

                        Vector2 size =
                            Vector2{.x = CELL_WIDTH, .y = CELL_HEIGHT};

                        DrawRectangleV(position, size, BEIGE);

                        // FIXME: DrawRectangleLinesEx(Rectangle{.x =
                        // position.x, .y = position.y, .width = CELL_WIDTH,
                        // .height = CELL_HEIGHT}, 1.12, RED);
                    }
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
