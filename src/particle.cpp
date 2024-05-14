#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <raylib.h>
#include <time.h>
#include <vector>
using std::vector;

// Although they are always the same value, we are redefining these macros.
// FIXME: Separate the config vars into a header files
#define WIDTH_R GetScreenWidth()
#define HEIGHT_R GetScreenHeight()

float gen_random_float(float min, float max) {
    typedef std::mt19937 Engine;
    typedef std::uniform_real_distribution<float> Distribution;

    auto r =
        std::bind(Distribution(min, max), Engine((unsigned int)time(NULL)));
    return r();
}

int gen_random_int(int min, int max) {
    typedef std::mt19937 Engine;
    typedef std::uniform_int_distribution<int> Distribution;

    auto r =
        std::bind(Distribution(min, max), Engine((unsigned int)time(NULL)));
    return r();
}

class Particle; // Forward declaration of Particle class

// I stole(yes) this function from:
// https://github.com/JMS55/sandbox/blob/master/src/behavior.rs
std::optional<std::pair<int, int>>
rand_available_neighbor(const int x, const int y, const int offsety,
                        vector<vector<Particle *>> &v) {
    const bool left_free = x - 1 >= 0 &&
                           (offsety == 0 || v[x - 1][y + offsety] == nullptr) &&
                           v[x - 1][y] == nullptr;

    const bool right_free =
        x + 1 < WIDTH_R && (offsety == 0 || v[x + 1][y + offsety] == nullptr) &&
        v[x + 1][y] == nullptr;

    if (left_free || right_free) {
        int newX = x;

        if (left_free && right_free) {
            // FIXME: newX = x - 1;

            if (gen_random_float(0.f, 1.f) < 0.5f) {
                newX = x - 1;
            } else {
                newX = x + 1;
            }
        } else if (left_free) {
            newX = x - 1;
        } else {
            newX = x + 1;
        }

        return std::pair<int, int>(newX, y + offsety);
    } else {
        return std::nullopt;
    }
}

class Particle {
  protected:
    int x;
    int y;
    Color color;

  public:
    Particle() {}

    void draw() {
        /* FIXME: REDUNDANT!!!
        Vector2 position = Vector2{.x = (float)CELL_WIDTH * this->c,
                                   .y = (float)CELL_HEIGHT * this->r};

        Vector2 size = Vector2{.x = (float)CELL_WIDTH, .y = (float)CELL_HEIGHT};

        DrawRectangleV(position, size, this->color);
        */

        DrawPixel(this->x, this->y, this->color);
    }

    void virtual update(vector<vector<Particle *>> &original,
                        vector<vector<Particle *>> &copy) = 0;
};

enum ParticleType {
    SAND,
    WATER,
    SMOKE,
    SOLID,
};

class Sand : public Particle {
  public:
    Sand(const int _r, const int _c) {
        this->x = _r;
        this->y = _c;
        this->color = ColorAlpha(
            ColorFromHSV(40.f + gen_random_float(2.f, 12.f), 1.f, 1.f), 255.f);
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old) {
        if (y + 1 < HEIGHT_R) {
            // move down if a spot is available
            if (!old[x][y + 1]) {
                original[x][y] = nullptr;
                original[x][y + 1] = this;
                this->y = this->y + 1;

                return;
            }

            if (std::optional<std::pair<int, int>> random_left_right =
                    rand_available_neighbor(x, y, 1, old);
                random_left_right.has_value()) {
                const int nx = random_left_right.value().first;
                const int ny = random_left_right.value().second;

                original[x][y] = nullptr;
                original[nx][ny] = this;
                this->x = nx;
                this->y = ny;

                return;
            }
        }

        original[this->x][this->y] =
            this; // otherwise just stay where we were before
    }
};

class Water : public Particle {
  public:
    Water(const int _x, const int _y) {
        this->x = _x;
        this->y = _y;
        this->color = BLUE;
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old) {
        if (y + 1 < HEIGHT_R) {
            // move down if a spot is available
            if (old[x][y + 1] == nullptr) {

                original[x][y] = nullptr;
                original[x][y + 1] = this;
                this->y = this->y + 1;

                return;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(x, y, 1, old);
                       random_left_right.has_value()) {
                const int nx = random_left_right.value().first;
                const int ny = random_left_right.value().second;

                original[x][y] = nullptr;
                original[nx][ny] = this;
                this->x = nx;
                this->y = ny;

                // std::cout<<"Sometihng\n";

                return;
            }
        }

        // don't move down just go left or right
        if (std::optional<std::pair<int, int>> random_left_right =
                rand_available_neighbor(x, y, 0, old);
            random_left_right.has_value()) {

            const int nx = random_left_right.value().first;
            const int ny = random_left_right.value().second;

            original[x][y] = nullptr;
            original[nx][ny] = this;

            this->x = nx;
            this->y = ny;

            return;
        }

        /*FIXME: remove later
        if (x + 1 < WIDTH_R && !old[x + 1][y]) {

            const int nx = x + 1;
            const int ny = y;

            original[x][y] = nullptr;
            original[nx][ny] = this;

            this->x = nx;
            this->y = ny;

            return;
        } else if (x - 1 >= 0 && !old[x - 1][y]) {

            const int nx = x - 1;
            const int ny = y;

            original[x][y] = nullptr;
            original[nx][ny] = this;

            this->x = nx;
            this->y = ny;

            return;
        }
        */

        original[this->x][this->y] =
            this; // otherwise just stay where we were before

        assert(original[x][y] != nullptr);
    }
};

class Smoke : public Particle {
  public:
    Smoke(const int _r, const int _c) {
        this->x = _r;
        this->y = _c;
        // 34°, 38%, 83%
        this->color = ColorAlpha(
            ColorFromHSV(30.f + gen_random_float(2.f, 12.f), .38f, .83f),
            gen_random_float(.15f, .20f));
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old) {

        if (y - 1 >= 0) {
            // move up if a spot is available
            if (!old[x][y - 1]) {
                original[x][y] = nullptr;
                original[x][y - 1] = this;
                this->y = this->y - 1;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(x, y, -1, old);
                       random_left_right.has_value()) {
                const int nr = random_left_right.value().first;
                const int nc = random_left_right.value().second;

                original[x][y] = nullptr;
                original[nr][nc] = this;
                this->x = nr;
                this->y = nc;
            }
        }

        original[x][y] = this; // otherwise just stay where we were before
    }
};

class Solid : public Particle {
  public:
    Solid(const int _r, const int _c) {
        this->x = _r;
        this->y = _c;
        // 20°, 70%, 50%
        this->color = ColorFromHSV(20.f, .70f, .50f);
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old) {

        // move down if a spot is available
        if (x + 1 < HEIGHT_R && !old[x + 1][y]) {
            original[x][y] = nullptr;
            original[x + 1][y] = this;
            this->x = this->x + 1;
        } else {                   // do nothing
            original[x][y] = this; // otherwise just stay where we were before
        }
    }
};
