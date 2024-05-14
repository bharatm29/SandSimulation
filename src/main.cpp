#include "./particle.cpp"
#include <raylib.h>
#include <string>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define PARTICLE_SPAWN_RADIUS 40

#define COLS WIDTH  /* / CELL_WIDTH */
#define ROWS HEIGHT /* / CELL_HEIGHT */

#define DEBUG_MODE false

vector<vector<Particle *>> updateState(vector<vector<Particle *>> &old_state) {
    vector<vector<Particle *>> next_state(COLS, vector<Particle *>(ROWS, nullptr));

    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS; r++) {
            if (old_state[c][r] != nullptr) {
                old_state[c][r]->update(next_state, old_state);
            }
        }
    }

    return next_state;
}

void set_keymaps(ParticleType &selectedParticle) {
    if (IsKeyPressed(KEY_W)) {
        selectedParticle = WATER;
    } else if (IsKeyPressed(KEY_C)) {
        selectedParticle = SMOKE;
    } else if (IsKeyPressed(KEY_S)) {
        selectedParticle = SAND;
    } else if (IsKeyPressed(KEY_H)) {
        selectedParticle = SOLID;
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(120);

    bool eraseMode = false;
    ParticleType selectedParticle = SAND;

    long long particles = 0;

    vector<vector<Particle *>> v(COLS, vector<Particle *>(ROWS, nullptr));

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
                    COLS, vector<Particle *>(ROWS, nullptr));
            }

            set_keymaps(selectedParticle);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                // const int cellx = GetMouseX() / CELL_WIDTH;
                // const int celly = GetMouseY() / CELL_HEIGHT;

                const int cellx = GetMouseX();
                const int celly = GetMouseY();

                if (cellx < 0 || celly < 0 || cellx >= WIDTH ||
                    celly >= HEIGHT) { // check if the cells are in bound or not
                                       // FIXME: REDUNDANT?
                    break;
                }

                for (int x = cellx;
                     x < WIDTH && x < cellx + PARTICLE_SPAWN_RADIUS; x += 2) {
                    for (int y = celly;
                         y < HEIGHT && y < celly + PARTICLE_SPAWN_RADIUS;
                         y += 2) {
                        if (eraseMode) {
                            v[x][y] = nullptr;
                        } else if (v[x][y] == nullptr) {
                            particles++;
                            switch (selectedParticle) {
                            case SAND:
                                v[x][y] = new Sand(x, y);
                                break;
                            case WATER:
                                v[x][y] = new Water(x, y);
                                break;
                            case SMOKE:
                                v[x][y] = new Smoke(x, y);
                                break;
                            case SOLID:
                                v[x][y] = new Solid(x, y);
                                break;
                            }
                        }
                    }
                }
            }

            for (int c = 0; c < COLS; c++) {
                for (int r = 0; r < ROWS; r++) {
                    if (v[c][r] != nullptr) {
                        v[c][r]->draw();
                    }
                }
            }

            if (DEBUG_MODE) {
                if (IsKeyDown(KEY_E)) {
                    v = updateState(v);
                }
            } else {
                v = updateState(v);
            }

            DrawText(std::to_string(particles).c_str(), 20, 20, 25, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
