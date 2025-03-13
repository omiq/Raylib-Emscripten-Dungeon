#ifndef RAYLIB_DISPLAY_H
#define RAYLIB_DISPLAY_H

#include <stdbool.h>
#include "raylib.h"

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 20

// Texture array to store all our character textures
extern Texture2D textures[128];  // ASCII range

// Initialize the Raylib display
void init_raylib_display(void);

// Close the Raylib display
void close_raylib_display(void);

// Draw a character at the specified grid position
void draw_char(int x, int y, unsigned char c);

// Draw a string at the specified grid position
void draw_string(int x, int y, const char* str);

// Clear the screen
void clear_screen(void);

// Update the display
void update_display(void);

// Get the current frame time
float get_frame_time(void);

// Check if window should close
bool window_should_close(void);

#endif // RAYLIB_DISPLAY_H 