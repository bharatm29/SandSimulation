#include "./particle.cpp"
#include <raylib.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 40
#define CELL_HEIGHT CELL_WIDTH

#define COLS WIDTH / CELL_WIDTH
#define ROWS HEIGHT / CELL_HEIGHT

#define DEBUG_MODE true

vector<vector<Particle *>> updateState(vector<vector<Particle *>> &vCopy) {
    vector<vector<Particle *>> v(ROWS, vector<Particle *>(COLS, nullptr));

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (vCopy[r][c] != nullptr) {
                vCopy[r][c]->update(v, vCopy, ROWS, COLS);
            }
        }
    }

    return v;
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(120);

    bool eraseMode = false;
    bool waterMode = false;
    unsigned int frameCounter = 0;

    vector<vector<Particle *>> v(ROWS, vector<Particle *>(COLS, nullptr));

    while (!WindowShouldClose()) {
        BeginDrawing();

        {
            ClearBackground(BLACK);

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                eraseMode = true;
            } else {
                eraseMode = false;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                v.clear();
                v = vector<vector<Particle *>>(
                    ROWS, vector<Particle *>(COLS, nullptr));
            }

            if (IsKeyPressed(KEY_SPACE)) {
                waterMode = !waterMode;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX() / CELL_WIDTH;
                const int celly = GetMouseY() / CELL_HEIGHT;

                if (eraseMode) {
                    v[celly][cellx] = nullptr;
                } else {

                    if (waterMode) {
                        v[celly][cellx] = new Water(celly, cellx);
                    } else {
                        v[celly][cellx] = new Sand(celly, cellx);
                    }
                }
            }

            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (v[r][c] != nullptr) {
                        v[r][c]->draw(CELL_WIDTH, CELL_HEIGHT);
                    }
                }
            }

            if (DEBUG_MODE) {
                if (IsKeyPressed(KEY_E)) {
                    v = updateState(v);
                }
            } else {
                v = updateState(v);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
