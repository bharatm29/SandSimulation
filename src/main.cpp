#include <raylib.h>

#define WIDTH 600
#define HEIGHT 600

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Sand Simulation");

    SetTargetFPS(60);

    if (!WindowShouldClose()) {
        SetMousePosition(WIDTH / 2, HEIGHT / 2);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();

        {
            ClearBackground(BLACK);
            DrawCircle(GetMouseX(), GetMouseY(), 20, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
