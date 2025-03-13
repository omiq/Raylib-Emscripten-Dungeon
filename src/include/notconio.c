#include "notconio.h"
#include "globals.h"
#include "../input/input.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void cputcxy_dark(int x, int y, unsigned char c);
void update_fov(int player_x, int player_y, int radius);

#include "../display/display.h"
#include "../include/globals.h"
#include "../logic/game_logic.h"

// Forward declaration
void update_fov(int player_x, int player_y, int radius);

// Dummy variable to satisfy ncurses references
void* stdscr = NULL;

// Frame counter for message timing
static int message_frame_counter = 0;
static bool message_visible = true;
static char current_message[256] = {0};

// Frame counter for momentary objects
static int momentary_object_frame_counter = 0;
static bool momentary_object_visible = false;
static struct {
    unsigned int x;
    unsigned int y;
    unsigned int tile;
    int duration_frames;
} current_momentary_object = {0};

// Text mode compatibility functions
void clrscr(void) {
    clear_screen();
}

void cputsxy(unsigned int x, unsigned int y, const char* str) {
    draw_string(x, y, str);
}

void cputcxy(unsigned int x, unsigned int y, char c) {
    draw_char(x, y, c);
}

void gotoxy(unsigned int x, unsigned int y) {
    // No-op in Raylib version
    (void)x;
    (void)y;
}

void cursor(int show) {
    // No-op in Raylib version
    (void)show;
}

void refresh(void) {
    // No-op in Raylib version - screen updates are handled by BeginDrawing/EndDrawing
}

void echo(void) {
    // No-op in Raylib version
}

void noecho(void) {
    // No-op in Raylib version
}

void nodelay(void* win, int val) {
    // No-op in Raylib version
    (void)win;
    (void)val;
}

void ungetch(int ch) {
    // No-op in Raylib version
    (void)ch;
}

int wgetch(void* win) {
    // No-op in Raylib version
    (void)win;
    return 0;
}

void wrefresh(void* win) {
    // No-op in Raylib version
    (void)win;
}

int cgetc(void) {
    // TODO: Implement proper key input using Raylib
    if (IsKeyPressed(KEY_ESCAPE)) return 27;
    if (IsKeyPressed(KEY_UP)) return 'w';
    if (IsKeyPressed(KEY_DOWN)) return 's';
    if (IsKeyPressed(KEY_LEFT)) return 'a';
    if (IsKeyPressed(KEY_RIGHT)) return 'd';
    if (IsKeyPressed(KEY_SPACE)) return ' ';
    if (IsKeyPressed(KEY_ENTER)) return '\n';
    return 0;
}

// Display functions
void output_message(void) {
    int titleSize = 30;
    
    // Store the message
    strncpy(current_message, output, sizeof(current_message) - 1);
    message_frame_counter = 0;
    message_visible = true;
    
    // Clear the output buffer
    output[1] = '\0';
}

// Call this in the main draw loop
void update_message(void) {
    if (strlen(current_message) > 0) {
        int titleSize = 30;
        message_frame_counter++;
        
        // Show message for 30 frames (0.5 seconds at 60fps)
        if (message_frame_counter < 30) {
            DrawText(current_message, 
                    (SCREEN_WIDTH - MeasureText(current_message, titleSize)) / 2, 
                    500, titleSize, WHITE);
        } else {
            // Clear the message
            current_message[0] = '\0';
        }
    }
}

void draw_screen(void) {
    // Draw whole screen
    int row, col;
//draw_whole_screen=true;
//screen_drawn = false;

    if (draw_whole_screen && screen_drawn == false) {
        for (row = 0; row < PLAYABLE_HEIGHT; row++) {
            for (col = 0; col < MAZE_WIDTH; col++) {
                cputcxy(col, row, get_map(col, row));
                visibility_map[MAP_WIDTH * row + col] = true;
            }
        }
        screen_drawn = true;
        printf("drawing whole screen");
    } else {

        
        // Update the screen around the player with a set radius 
        update_fov(player_x, player_y, 2);
        
    }

        

}

void draw_momentary_object(unsigned int obj_old_x, unsigned int obj_old_y, 
                          unsigned int obj_x, unsigned int obj_y, 
                          unsigned int obj_tile, int delay) {
    // Store the object info
    current_momentary_object.x = obj_x;
    current_momentary_object.y = obj_y;
    current_momentary_object.tile = obj_tile;
    current_momentary_object.duration_frames = delay; // Now in frames instead of ms
    momentary_object_frame_counter = 0;
    momentary_object_visible = true;
    
    // Replace old tile immediately
    cputcxy(obj_old_x, obj_old_y, get_map(obj_old_x, obj_old_y));
}

// Call this in the main draw loop
void update_momentary_object(void) {
    if (momentary_object_visible) {
        momentary_object_frame_counter++;
        
        if (momentary_object_frame_counter < current_momentary_object.duration_frames) {
            // Draw the object
            cputcxy(current_momentary_object.x, 
                   current_momentary_object.y, 
                   current_momentary_object.tile);
        } else {
            // Clear the object
            cputcxy(current_momentary_object.x, 
                   current_momentary_object.y, 
                   get_map(current_momentary_object.x, current_momentary_object.y));
            momentary_object_visible = false;
        }
    }
}

void update_fov(int player_x, int player_y, int radius) {
    cputcxy(old_x, old_y, ' ');

    for (int y = player_y - radius; y <= player_y + radius; y++) {
        for (int x = player_x - radius; x <= player_x + radius; x++) {
            // Check if coordinates are within map bounds
            if (y >= 0 && y < PLAYABLE_HEIGHT && x >= 0 && x < MAP_WIDTH) {
                // Mark tile as visible
                
                // Draw the current FOV with full brightness
                unsigned char tile = get_map(x, y);
                if (tile != 0) {
                    cputcxy(x, y, tile);
                    visibility_map[MAP_WIDTH * y + x] = true;

                }
            }
        }
    }
    
    cputcxy(player_x, player_y, '@');

    // Draw previously revealed tiles outside FOV in darker color
    for (int y = 0; y < PLAYABLE_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Skip tiles in current FOV
            if (abs(x - player_x) <= radius && abs(y - player_y) <= radius) {
                continue;
            }
            
            // If tile was previously revealed but not in current FOV
            if (visibility_map[MAP_WIDTH * y + x]) {
                unsigned char tile = get_map(x, y);
                if (tile != 0) {
                    cputcxy_dark(x, y, tile);
                }
            }
        }
    }
} 

void cputcxy_dark(int x, int y, unsigned char c) {
    if (c == ' ') c = '.';
    Color darkColor = {128, 128, 128, 255};  // Gray color for revealed but not in FOV
    if (textures[c].id != 0) {
        DrawTexture(textures[c], x * TILE_SIZE, y * TILE_SIZE, darkColor);
    } else if (c == '.') {
        DrawTexture(textures['.'], x * TILE_SIZE, y * TILE_SIZE, darkColor);
    }
}


