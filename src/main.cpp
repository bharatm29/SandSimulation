#include "ffmpeg.cpp"
#include "particle.cpp"
#include <raylib.h>
#include <unistd.h>
#include <vector>

// TODO: #include "config.h"; So I included it once in particle.cpp, since it
// was giving me redefinition warning.

using std::vector;

vector<vector<Particle *>> updateState(vector<vector<Particle *>> &old_state) {
    vector<vector<Particle *>> next_state(WIDTH,
                                          vector<Particle *>(HEIGHT, nullptr));

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (old_state[x][y] != nullptr) {
                old_state[x][y]->update(next_state, old_state);
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
    FFMPEG *ffmpeg = ffmpeg_start_rendering(WIDTH, HEIGHT, FPS, 0);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(FPS);

    bool eraseMode = false;
    bool renderMode = false;
    ParticleType selectedParticle = SAND;

    unsigned int particle_spawn_radius = 10;

    vector<vector<Particle *>> particles(WIDTH,
                                         vector<Particle *>(HEIGHT, nullptr));

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

            // clear the particles on pressing ENTER
            if (IsKeyPressed(KEY_ENTER)) {
                particles.clear();
                particles = vector<vector<Particle *>>(
                    WIDTH, vector<Particle *>(HEIGHT, nullptr));
            }

            // start the render on pressing R
            if (IsKeyPressed(KEY_R)) {
                renderMode = !renderMode;
            }

            // increase/decrease particle brush size with +/-
            if (IsKeyPressed(KEY_MINUS)) {
                if (particle_spawn_radius > 5) {
                    particle_spawn_radius -= 5;
                }
            }
            if (IsKeyDown(KEY_LEFT_SHIFT) &&
                IsKeyPressed(KEY_EQUAL)) { // SHIFT = is +
                if (particle_spawn_radius < 30) {
                    particle_spawn_radius += 5;
                }
            }

            // set keymaps to draw different particles
            set_keymaps(selectedParticle);

            // draw particle brush circle
            DrawCircleLines(GetMouseX(), GetMouseY(), particle_spawn_radius,
                            WHITE);

            // draw particles on LEFT CLICK
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                const int cellx = GetMouseX();
                const int celly = GetMouseY();

                // FIXME: We are only drawing for one sector of the circle. Fix
                // to draw for the whole sector
                // Possible Fixes: https://stackoverflow.com/a/40779613
                // https://www.reddit.com/r/askmath/comments/u81h7t/how_do_i_get_all_the_grid_points_inside_a_circle/
                for (int x = cellx;
                     x < WIDTH && x < cellx + particle_spawn_radius; x += 2) {
                    for (int y = celly;
                         y < HEIGHT && y < celly + particle_spawn_radius;
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
            for (int x = 0; x < WIDTH; x++) {
                for (int y = 0; y < HEIGHT; y++) {
                    if (particles[x][y] != nullptr) {
                        particles[x][y]->draw();
                    }
                }
            }

            // if the debug mode is on, update on pressing down 'e' else call
            // update every time
            if (DEBUG_MODE) {
                if (IsKeyDown(KEY_E)) {
                    particles = updateState(particles);
                }
            } else {
                particles = updateState(particles);
            }
        }

        EndDrawing();

        // send frames to FFMPEG if in render mode
        if (renderMode) {
            Image image = LoadImageFromScreen();
            ffmpeg_send_frame(ffmpeg, image.data, WIDTH, HEIGHT);
            UnloadImage(image);
        }
    }

    CloseWindow();

    // free all the particles before exiting
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            delete particles[x][y];
        }
    }

    ffmpeg_end_rendering(ffmpeg);
}
