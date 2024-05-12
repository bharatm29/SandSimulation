#include <iostream>
#include <raylib.h>
#include <vector>
using std::vector;

class Particle {
  protected:
    int r;
    int c;
    Color color;

  public:
    Particle() {}
    virtual void draw(const int CELL_WIDTH, const int CELL_HEIGHT) {}
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

    void draw(const int CELL_WIDTH, const int CELL_HEIGHT) {
        Vector2 position = Vector2{.x = (float)CELL_WIDTH * this->c,
                                   .y = (float)CELL_HEIGHT * this->r};

        Vector2 size = Vector2{.x = (float)CELL_WIDTH, .y = (float)CELL_HEIGHT};

        DrawRectangleV(position, size, this->color);
    }

    void update(vector<vector<Particle *>> &original,
                vector<vector<Particle *>> &old, const int ROWS,
                const int COLS) {

        if (!old[r + 1][c]) {
            original[r][c] = nullptr;
            original[r + 1][c] = new Sand(r + 1, c);
        } else if (old[r][c]) {
            original[r][c] = new Sand(r, c);
        }

        std::cout << "I was called: " << original[r][c] << " " << old[r][c]
                  << std::endl;

        /*else if (r + 1 < ROWS && c - 1 < COLS &&
                   old[r + 1][c - 1] == nullptr) {
            original[r][c] = nullptr;
            original[r + 1][c - 1] = this;
        } else if (r + 1 < ROWS && c + 1 < COLS &&
                   old[r + 1][c + 1] == nullptr) {
            original[r][c] = nullptr;
            original[r + 1][c + 1] = this;
        }
        */
    }
};
