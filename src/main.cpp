#include <iostream>

#include "../include/raylib.h"

#include "../include/gui.hpp"

int main(void)
{
  constexpr int windowWidth = 800;
  constexpr int windowHeight = 450;

  InitWindow(windowWidth, windowHeight, "GUI");
  SetTargetFPS(60);

  int monitor = GetCurrentMonitor();
  float monitorWidth = GetMonitorWidth(monitor);
  float monitorHeight = GetMonitorHeight(monitor);
  float scale = (float)monitorWidth/1000;

  float scaledWindowWidth = windowWidth*scale;
  float scaledWindowHeight = windowHeight*scale;

  SetWindowSize(scaledWindowWidth, scaledWindowHeight);
  SetWindowPosition((monitorWidth/2)-(scaledWindowWidth/2), (monitorHeight/2)-(scaledWindowHeight/2));

  GUI::MouseState mouseState = { 0 };

  // Setup GUI styles here
  // ---------------------------
  

  // Initalizie GUI components here
  // ----------------------------------

  while (!WindowShouldClose())
  {
    mouseState.position = GetMousePosition();
    mouseState.clicked = false;
    mouseState.cursor = MOUSE_CURSOR_DEFAULT;
    SetMouseCursor(mouseState.cursor);

    BeginDrawing();
    ClearBackground(BLACK);

    // Update and render GUI components here
    // -----------------------------------------

    EndDrawing();
  }

  CloseWindow();
}
