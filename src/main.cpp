#include "./particle.cpp"
#include <functional>
#include <random>
#include <raylib.h>
#include <time.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 5
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

vector<vector<Particle *>> updateState(vector<vector<Particle *>>& vCopy) {
    vector<vector<Particle *>> v(ROWS, vector<Particle*>(COLS, nullptr));

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
                } else

                    /*
                    if (waterMode) {
                        v[celly][cellx] = new Particle();
                    } else {
                    */
                    v[celly][cellx] = new Sand(celly, cellx);
                // }
            }

            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (v[r][c] != nullptr) {
                        v[r][c]->draw(CELL_WIDTH, CELL_HEIGHT);
                    }
                }
            }

            v = updateState(v);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
