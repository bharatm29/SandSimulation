#include <iostream>
#include <raylib.h>
#include <vector>
using std::vector;

/*
/// Returns a random available neighbor of (x, y) if any.
/// Searches x-1 and x+1 at a y-coordinate of y + y_offset
fn rand_available_neighbor(
    sandbox: &mut Sandbox,
    x: usize,
    y: usize,
    y_offset: isize,
) -> Option<(usize, usize)> {
    // Check whether the left and right paths to candidate cells are free
    let left_free = x != 0
        && (y_offset == 0 || sandbox[x - 1][(y as isize + y_offset) as
usize].is_none())
        && sandbox[x - 1][y].is_none();
    let right_free = x != SANDBOX_WIDTH - 1
        && (y_offset == 0 || sandbox[x + 1][(y as isize + y_offset) as
usize].is_none())
        && sandbox[x + 1][y].is_none();
    if left_free || right_free {
        // If both are free, pick one at random, else pick the free one
        let diagonal_x = if left_free && right_free {
            [x - 1, x + 1][sandbox.rng.gen_range(0..2)]
        } else if left_free {
            x - 1
        } else {
            x + 1
        };
        Some((diagonal_x, (y as isize + y_offset) as usize))
    } else {
        None
    }
}
*/

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

        // move down if a spot is available
        if (r + 1 < ROWS && !old[r + 1][c]) {
            original[r][c] = nullptr;
            original[r + 1][c] = this;
            this->r = this->r + 1;
        } else if (r + 1 < ROWS && c - 1 < COLS &&
                   old[r + 1][c - 1] ==
                       nullptr) { // check for right diagonal neighbour
            original[r][c] = nullptr;
            original[r + 1][c - 1] = this;
            this->r = this->r + 1;
            this->c = this->c - 1;
        } else if (r + 1 < ROWS && c + 1 < COLS &&
                   old[r + 1][c + 1] ==
                       nullptr) { // check for right diagonal neighbour
            original[r][c] = nullptr;
            original[r + 1][c + 1] = this;
            this->r = this->r + 1;
            this->c = this->c + 1;
        } else { // otherwise just stay where we were before
            original[this->r][this->c] = this;
        }
    }
};
