#include <functional>
#include <optional>
#include <random>
#include <raylib.h>
#include <time.h>
#include <vector>
using std::vector;

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
rand_available_neighbor(const int r, const int c, const int offsetr,
                        vector<vector<Particle *>> &v) {
    const bool left_free = r != 0 &&
                           (offsetr == 0 || v[r + offsetr][c - 1] == nullptr) &&
                           v[r][c - 1] == nullptr;

    const bool right_free =
        r != 0 && (offsetr == 0 || v[r + offsetr][c + 1] == nullptr) &&
        v[r][c + 1] == nullptr;

    if (left_free || right_free) {
        int newC = c;

        if (left_free && right_free) {
            if (gen_random_float(0.f, 1.f) < 0.5f) {
                newC = c - 1;
            } else {
                newC = c + 1;
            }
        } else if (left_free) {
            newC = c - 1;
        } else {
            newC = c + 1;
        }

        return std::pair<int, int>(r + offsetr, newC);
    } else {
        return std::nullopt;
    }
}

class Particle {
  protected:
    int r;
    int c;
    Color color;

  public:
    Particle() {}

    void draw(const int CELL_WIDTH, const int CELL_HEIGHT) {
        Vector2 position = Vector2{.x = (float)CELL_WIDTH * this->c,
                                   .y = (float)CELL_HEIGHT * this->r};

        Vector2 size = Vector2{.x = (float)CELL_WIDTH, .y = (float)CELL_HEIGHT};

        DrawRectangleV(position, size, this->color);
    }

    virtual void update(vector<vector<Particle *>> &original,
                        vector<vector<Particle *>> &copy, const int ROWS,
                        const int COLS) {}
};

class Sand : public Particle {
  public:
    Sand(const int _r, const int _c) {
        this->r = _r;
        this->c = _c;
        this->color = GOLD;
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old, const int ROWS,
                const int COLS) {

        if (r + 1 < ROWS) {
            // move down if a spot is available
            if (!old[r + 1][c]) {
                original[r][c] = nullptr;
                original[r + 1][c] = this;
                this->r = this->r + 1;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(r, c, 1, old);
                       random_left_right.has_value()) {
                const int nr = random_left_right.value().first;
                const int nc = random_left_right.value().second;

                original[r][c] = nullptr;
                original[nr][nc] = this;
                this->r = nr;
                this->c = nc;
            }
        }

        original[this->r][this->c] =
            this; // otherwise just stay where we were before
    }
};

class Water : public Particle {
  public:
    Water(const int _r, const int _c) {
        this->r = _r;
        this->c = _c;
        this->color = BLUE;
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old, const int ROWS,
                const int COLS) {

        if (r + 1 < ROWS) {
            // move down if a spot is available
            if (!old[r + 1][c]) {
                original[r][c] = nullptr;
                original[r + 1][c] = this;
                this->r = this->r + 1;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(r, c, 1, old);
                       random_left_right.has_value()) {
                const int nr = random_left_right.value().first;
                const int nc = random_left_right.value().second;

                original[r][c] = nullptr;
                original[nr][nc] = this;
                this->r = nr;
                this->c = nc;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(r, c, 0, old);
                       random_left_right.has_value()) {
                const int nr = random_left_right.value().first;
                const int nc = random_left_right.value().second;

                original[r][c] = nullptr;
                original[nr][nc] = this;
                this->r = nr;
                this->c = nc;
            }
            // FIXME: Change the update such that the water keeps moving the
            // same direction untill it can and not change the direction mid way
            // due to randomness
        }

        original[this->r][this->c] =
            this; // otherwise just stay where we were before
    }
};
