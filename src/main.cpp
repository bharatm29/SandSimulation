#include "./particle.cpp"
#include <raylib.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define CELL_WIDTH 5
#define CELL_HEIGHT CELL_WIDTH

#define COLS WIDTH / CELL_WIDTH
#define ROWS HEIGHT / CELL_HEIGHT

#define DEBUG_MODE false

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

void set_keymaps(ParticleType &selectedParticle) {
    if (IsKeyPressed(KEY_W)) {
        selectedParticle = WATER;
    } else if (IsKeyPressed(KEY_C)) {
        selectedParticle = SMOKE;
    } else if (IsKeyPressed(KEY_S)) {
        selectedParticle = SAND;
    } else if (IsKeyPressed(KEY_H)) {
        selectedParticle =  SOLID;
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(120);

    bool eraseMode = false;
    ParticleType selectedParticle = SAND;
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

            set_keymaps(selectedParticle);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX() / CELL_WIDTH;
                const int celly = GetMouseY() / CELL_HEIGHT;

                if (celly >= ROWS || cellx >= COLS) { // check if the cells are in bound or not
                    break;
                }

                if (eraseMode) {
                    v[celly][cellx] = nullptr;
                } else {
                    switch (selectedParticle) {
                    case WATER:
                        v[celly][cellx] = new Water(celly, cellx);
                        break;
                    case SMOKE:
                        v[celly][cellx] = new Smoke(celly, cellx);
                        break;
                    case SAND:
                        v[celly][cellx] = new Sand(celly, cellx);
                        break;
                    case SOLID:
                        v[celly][cellx] = new Solid(celly, cellx);
                        break;
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
