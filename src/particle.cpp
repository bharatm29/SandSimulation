#include <functional>
#include <optional>
#include <random>
#include <raylib.h>
#include <time.h>
#include <vector>
using std::vector;

// Although they are always the same value, we are redefining these macros.
// FIXME: Separate the config vars into a header files
const int WIDTH_R = GetScreenWidth();
const int HEIGHT_R = GetScreenHeight();

float gen_random_float(float min, float max) {
    // source: https://stackoverflow.com/a/78276914
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

class Particle; // Forward declaration of abstract Particle class

bool isEmptyCell(const int x, const int y,
                 vector<vector<Particle *>> &old_state,
                 vector<vector<Particle *>> &next_state) {
    return (old_state[x][y] == nullptr && next_state[x][y] == nullptr);
}

std::optional<std::pair<int, int>>
rand_available_neighbor(const int x, const int y, const int offsety,
                        vector<vector<Particle *>> &old_state,
                        vector<vector<Particle *>> &next_state) {
    const bool left_free =
        x - 1 >= 0 &&
        (offsety == 0 ||
         isEmptyCell(x - 1, y + offsety, old_state, next_state)) &&
        (isEmptyCell(x - 1, y, old_state, next_state));

    const bool right_free =
        x + 1 < WIDTH_R &&
        (offsety == 0 ||
         isEmptyCell(x + 1, y + offsety, old_state, next_state)) &&
        (isEmptyCell(x + 1, y, old_state, next_state));

    if (left_free || right_free) {
        int newX = x;

        if (left_free && right_free) {
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

enum ParticleType {
    SAND,
    WATER,
    SMOKE,
    SOLID,
};

class Particle {
  protected:
    int m_x;
    int m_y;
    Color m_color;

  public:
    ParticleType m_type;

  public:
    Particle() {}
    virtual ~Particle() {}

    void draw() {
        /* FIXME: REDUNDANT!!!
        Vector2 position = Vector2{.x = (float)CELL_WIDTH * this->c,
                                   .y = (float)CELL_HEIGHT * this->r};

        Vector2 size = Vector2{.x = (float)CELL_WIDTH, .y = (float)CELL_HEIGHT};

        DrawRectangleV(position, size, this->color);
        */

        DrawPixel(this->m_x, this->m_y, this->m_color);
    }

    void virtual update(vector<vector<Particle *>> &next_state,
                        vector<vector<Particle *>> &old_state) = 0;
};

class Sand : public Particle {
  public:
    Sand(const int _r, const int _c) {
        this->m_x = _r;
        this->m_y = _c;
        this->m_color = ColorAlpha(
            ColorFromHSV(40.f + gen_random_float(2.f, 12.f), 1.f, 1.f), 255.f);
        this->m_type = SAND;
    }

    void update(vector<vector<Particle *>> &next_state,
                vector<vector<Particle *>> &old_state) {
        if (m_y + 1 < HEIGHT_R) {
            // move down if a spot is available
            if (isEmptyCell(m_x, m_y + 1, old_state, next_state)) {
                next_state[m_x][m_y] = nullptr;
                next_state[m_x][m_y + 1] = this;
                this->m_y = this->m_y + 1;

                return;
            }

            if (std::optional<std::pair<int, int>> random_left_right =
                    rand_available_neighbor(m_x, m_y, 1, old_state, next_state);
                random_left_right.has_value()) {
                const int nx = random_left_right.value().first;
                const int ny = random_left_right.value().second;

                next_state[m_x][m_y] = nullptr;
                next_state[nx][ny] = this;
                this->m_x = nx;
                this->m_y = ny;

                return;
            }
        }

        next_state[this->m_x][this->m_y] =
            this; // otherwise just stay where we were before
    }
};

class Water : public Particle {
  public:
    Water(const int _x, const int _y) {
        this->m_x = _x;
        this->m_y = _y;
        this->m_color = BLUE;
        this->m_type = WATER;
    }

    void update(vector<vector<Particle *>> &next_state,
                vector<vector<Particle *>> &old_state) {
        if (m_y + 1 < HEIGHT_R) {
            // move down if a spot is available
            if (isEmptyCell(m_x, m_y + 1, old_state, next_state)) {

                // next_state[m_x][m_y] = nullptr;
                next_state[m_x][m_y + 1] = this;
                this->m_y = this->m_y + 1;

                return;
            } else if (std::optional<std::pair<int, int>> rand_neighbor =
                           rand_available_neighbor(m_x, m_y, 1, old_state,
                                                   next_state);
                       rand_neighbor.has_value()) {
                const int new_x = rand_neighbor.value().first;
                const int new_y = rand_neighbor.value().second;

                // next_state[m_x][m_y] = nullptr;
                next_state[new_x][new_y] = this;

                this->m_x = new_x;
                this->m_y = new_y;

                return;
            }
        }

        // don't move down just go left or right
        if (std::optional<std::pair<int, int>> random_neighbor =
                rand_available_neighbor(m_x, m_y, 0, old_state, next_state);
            random_neighbor.has_value()) {

            const int new_x = random_neighbor.value().first;
            const int new_y = random_neighbor.value().second;

            // next_state[m_x][m_y] = nullptr;
            next_state[new_x][new_y] = this;

            this->m_x = new_x;
            this->m_y = new_y;

            return;
        }

        next_state[this->m_x][this->m_y] =
            this; // otherwise just stay where we were before

        // FIXME: (REDUNDANT??) assert(next_state[m_x][m_y] != nullptr);
    }
};

class Smoke : public Particle {
  public:
    Smoke(const int _r, const int _c) {
        this->m_x = _r;
        this->m_y = _c;
        // 34°, 38%, 83%
        this->m_color = ColorAlpha(
            ColorFromHSV(30.f + gen_random_float(2.f, 12.f), .38f, .83f),
            gen_random_float(.15f, .20f));
        this->m_type = SMOKE;
    }

    void update(vector<vector<Particle *>> &next_state,
                vector<vector<Particle *>> &old_state) {

        if (m_y - 1 >= 0) {
            // move up if a spot is available
            if (isEmptyCell(m_x, m_y - 1, old_state, next_state)) {
                next_state[m_x][m_y] = nullptr;
                next_state[m_x][m_y - 1] = this;
                this->m_y = this->m_y - 1;
            } else if (std::optional<std::pair<int, int>> random_left_right =
                           rand_available_neighbor(m_x, m_y, -1, old_state,
                                                   next_state);
                       random_left_right.has_value()) {
                const int nr = random_left_right.value().first;
                const int nc = random_left_right.value().second;

                next_state[m_x][m_y] = nullptr;
                next_state[nr][nc] = this;
                this->m_x = nr;
                this->m_y = nc;
            }
        }

        next_state[m_x][m_y] = this; // otherwise just stay where we were before
    }
};

class Solid : public Particle {
  public:
    Solid(const int _r, const int _c) {
        this->m_x = _r;
        this->m_y = _c;
        // 20°, 70%, 50%
        this->m_color = ColorFromHSV(20.f, .70f, .50f);
        this->m_type = SOLID;
    }

    void update(vector<vector<Particle *>> &next_state,
                vector<vector<Particle *>> &old_state) {

        // move down if a spot is available
        if (m_y + 1 < HEIGHT_R &&
            isEmptyCell(m_x, m_y + 1, old_state, next_state)) {
            next_state[m_x][m_y] = nullptr;
            next_state[m_x][m_y + 1] = this;
            this->m_y = this->m_y + 1;
        } else { // do nothing
            next_state[m_x][m_y] =
                this; // otherwise just stay where we were before
        }
    }
};
