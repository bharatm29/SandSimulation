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

void updateState(vector<vector<int>> &v) {
    vector<vector<int>> vCopy = v;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (vCopy[r][c] == 1) {
                int nr = r;
                int nc = c;

                std::pair<int, int> d0, d1;

                if (gen_random_float(0.0f, 1.0f) < 0.5f) {
                    d0 = {r + 1, c + 1};
                    d1 = {r + 1, c + (-1)};
                } else {
                    d0 = {r + 1, c + (-1)};
                    d1 = {r + 1, c + 1};
                }

                if (r + 1 < ROWS && vCopy[r + 1][c] == 0) {
                    v[r][c] = 0;
                    nr = r + 1;
                    nc = c;
                } else if ((d0.first < ROWS && d0.second < COLS) &&
                           vCopy[d0.first][d0.second] == 0) {
                    v[r][c] = 0;
                    nr = d0.first;
                    nc = d0.second;
                } else if ((d1.first < ROWS && d1.second < COLS) &&
                           vCopy[d1.first][d1.second] == 0) {
                    v[r][c] = 0;
                    nr = d1.first;
                    nc = d1.second;
                }

                v[nr][nc] = 1;
            } else if (vCopy[r][c] == 2) {
                int nr = r;
                int nc = c;

                std::pair<int, int> d0, d1, d2, d3;

                if (gen_random_float(0.0f, 1.0f) < 0.5f) {
                    d0 = {r + 1, c + 1};
                    d1 = {r + 1, c + (-1)};
                    d2 = {r, c + 1};
                    d3 = {r, c + (-1)};
                } else {
                    d0 = {r + 1, c + (-1)};
                    d1 = {r + 1, c + 1};
                    d2 = {r, c + (-1)};
                    d3 = {r, c + 1};
                }

                if (r + 1 < ROWS && vCopy[r + 1][c] == 0) {
                    v[r][c] = 0;
                    nr = r + 1;
                    nc = c;
                } else if ((d0.first < ROWS && d0.second < COLS) &&
                           vCopy[d0.first][d0.second] == 0) {
                    v[r][c] = 0;
                    nr = d0.first;
                    nc = d0.second;
                } else if ((d1.first < ROWS && d1.second < COLS) &&
                           vCopy[d1.first][d1.second] == 0) {
                    v[r][c] = 0;
                    nr = d1.first;
                    nc = d1.second;
                } else if ((r != ROWS - 1 && d2.first < ROWS && d2.second < COLS) &&
                           vCopy[d2.first][d2.second] == 0) {
                    v[r][c] = 0;
                    nr = d2.first;
                    nc = d2.second;
                } else if ((r != ROWS - 1 && d3.first < ROWS && d3.second < COLS) &&
                           vCopy[d3.first][d3.second] == 0) {
                    v[r][c] = 0;
                    nr = d3.first;
                    nc = d3.second;
                }

                v[nr][nc] = 2;
            }
        }
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(120);

    bool eraseMode = false;
    bool waterMode = false;
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

            if (IsKeyPressed(KEY_ENTER)) {
                v.clear();
                v = vector<vector<int>>(ROWS, vector<int>(COLS, 0));
            }

            if (IsKeyPressed(KEY_SPACE)) {
                waterMode = !waterMode;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX() / CELL_WIDTH;
                const int celly = GetMouseY() / CELL_HEIGHT;

                if (eraseMode) {
                    v[celly][cellx] = 0;
                } else

                    if (waterMode) {
                    v[celly][cellx] = 2;
                } else {
                    v[celly][cellx] = 1;
                }
            }

            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    Vector2 position = Vector2{.x = (float)CELL_WIDTH * c,
                                               .y = (float)CELL_HEIGHT * r};

                    Vector2 size = Vector2{.x = CELL_WIDTH, .y = CELL_HEIGHT};

                    if (v[r][c] == 1) {
                        DrawRectangleV(position, size, GOLD);
                    } else if (v[r][c] == 2) {
                        DrawRectangleV(position, size, BLUE);
                    }
                }
            }

            updateState(v);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
