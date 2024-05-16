#include "./particle.cpp"
#include <raylib.h>
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 600

#define PARTICLE_SPAWN_RADIUS 40

#define COLS WIDTH  /* / CELL_WIDTH */
#define ROWS HEIGHT /* / CELL_HEIGHT */

#define DEBUG_MODE false

vector<vector<Particle *>> updateState(vector<vector<Particle *>> &old_state) {
    vector<vector<Particle *>> next_state(COLS,
                                          vector<Particle *>(ROWS, nullptr));

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

    vector<vector<Particle *>> particles(COLS,
                                         vector<Particle *>(ROWS, nullptr));

    while (!WindowShouldClose()) {
        BeginDrawing();

        {
            ClearBackground(BLACK);

            // enable erase mode when LSHIFT is being pressed down
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                eraseMode = true;
            } else {
                eraseMode = false;
            }

            // clear the particles on 'enter'
            if (IsKeyPressed(KEY_ENTER)) {
                particles.clear();
                particles = vector<vector<Particle *>>(
                    COLS, vector<Particle *>(ROWS, nullptr));
            }

            // set keymaps to draw different particles
            set_keymaps(selectedParticle);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX();
                const int celly = GetMouseY();

                // draw particles in a circle with center at the mouse (x, y) and predefined radius
                for (int x = cellx;
                     x < WIDTH && x < cellx + PARTICLE_SPAWN_RADIUS; x += 2) {
                    for (int y = celly;
                         y < HEIGHT && y < celly + PARTICLE_SPAWN_RADIUS;
                         y += 2) {
                        if (eraseMode) {
                            delete particles[x][y];
                            particles[x][y] = nullptr;
                        } else if (particles[x][y] == nullptr) {
                            switch (selectedParticle) {
                            case SAND:
                                particles[x][y] = new Sand(x, y);
                                break;
                            case WATER:
                                particles[x][y] = new Water(x, y);
                                break;
                            case SMOKE:
                                particles[x][y] = new Smoke(x, y);
                                break;
                            case SOLID:
                                particles[x][y] = new Solid(x, y);
                                break;
                            }
                        }
                    }
                }
            }

            // draw each particle
            for (int c = 0; c < COLS; c++) {
                for (int r = 0; r < ROWS; r++) {
                    if (particles[c][r] != nullptr) {
                        particles[c][r]->draw();
                    }
                }
            }

            // if the debug mode is on, update on pressing down 'e' else call update every time
            if (DEBUG_MODE) {
                if (IsKeyDown(KEY_E)) {
                    particles = updateState(particles);
                }
            } else {
                particles = updateState(particles);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
