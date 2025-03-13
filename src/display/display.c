#include "raylib.h"
#include "display.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "../include/resource_dir.h"

#define TILE_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_WIDTH 40
#define GRID_HEIGHT 30

// Texture array to store all our character textures
Texture2D textures[128];  // ASCII range
static bool textures_loaded = false;

// Load all character textures
static void load_textures(void) {
    // Find the resources directory
    if (!SearchAndSetResourceDir("resources")) {
        printf("Failed to find resources directory. Please make sure the resources folder is in the same directory as the executable.\n");
        #ifdef __EMSCRIPTEN__
        printf("Emscripten build: Resources should be at /resources\n");
        #endif
        CloseWindow();
    }

    if (textures_loaded) return;
    
    char filename[256];
    int loaded_count = 0;
    
    for (int i = 0; i < 128; i++) {
        // Convert ASCII character to filename
        if (i == '@') GetResourcePath("@.png", filename, sizeof(filename));
        else if (i == '#') GetResourcePath("#.png", filename, sizeof(filename));
        else if (i == '$') GetResourcePath("$.png", filename, sizeof(filename));
        else if (i == '*') GetResourcePath("*.png", filename, sizeof(filename));
        else if (i == '+') GetResourcePath("+.png", filename, sizeof(filename));
        else if (i == '-') GetResourcePath("-.png", filename, sizeof(filename));
        else if (i == '|') GetResourcePath("|.png", filename, sizeof(filename));
        else if (i == 'g') GetResourcePath("g.png", filename, sizeof(filename));
        else if (i == 'h') GetResourcePath("h.png", filename, sizeof(filename));
        else if (i == 'i') GetResourcePath("i.png", filename, sizeof(filename));
        else if (i == 'k') GetResourcePath("k.png", filename, sizeof(filename));
        else if (i == 'r') GetResourcePath("r.png", filename, sizeof(filename));
        else if (i == 's') GetResourcePath("s.png", filename, sizeof(filename));
        else if (i == '.') GetResourcePath("..png", filename, sizeof(filename));
        else if (i == 'O') GetResourcePath("O.png", filename, sizeof(filename));
        else if (i == '<') GetResourcePath("stabl.png", filename, sizeof(filename));
        else if (i == '>') GetResourcePath("stabr.png", filename, sizeof(filename));
        
        else {
            // For other characters, use a default texture or skip
            continue;
        }
        
        printf("Loading texture: %s\n", filename);
        textures[i] = LoadTexture(filename);
        if (textures[i].id > 0) {
            loaded_count++;
            printf("Successfully loaded texture for '%c' (id: %u, width: %d, height: %d)\n", 
                   i, textures[i].id, textures[i].width, textures[i].height);
        } else {
            printf("Failed to load texture for '%c' from %s\n", i, filename);
        }
    }
    
    printf("Loaded %d textures successfully\n", loaded_count);
    textures_loaded = true;
}

// Initialize Raylib display
void init_raylib_display(void) {

    // Set to 60 frames-per-second
	SetTargetFPS(60);               

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Dungeon");
    
    printf("Window initialized, loading textures...\n");
    load_textures();
    printf("Textures loaded\n");
}

// Close Raylib display
void close_raylib_display(void) {
    for (int i = 0; i < 128; i++) {
        if (textures[i].id != 0) {
            UnloadTexture(textures[i]);
        }
    }
    CloseWindow();
}

// Draw a character at the specified grid position
void draw_char(int x, int y, unsigned char c) {
    if (textures[c].id != 0) {
        DrawTexture(textures[c], x * TILE_SIZE, y * TILE_SIZE, WHITE);
    } else if (c == ' ' || c == '.') {
        // Draw floor texture for empty spaces
        if (textures['.'].id != 0) {
            DrawTexture(textures['.'], x * TILE_SIZE, y * TILE_SIZE, WHITE);
        }
    }
}

// Draw a string at the specified grid position
void draw_string(int x, int y, const char* str) {
    int current_x = x;
    for (const char* c = str; *c != '\0'; c++) {
        draw_char(current_x, y, *c);
        current_x++;
    }
}

// Clear the screen
void clear_screen(void) {
    ClearBackground(BLACK);
}

// Get the current frame time
float get_frame_time(void) {
    return GetFrameTime();
}

// Check if window should close
bool window_should_close(void) {
    return WindowShouldClose();
} 