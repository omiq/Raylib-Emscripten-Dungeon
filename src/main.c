// Raylib and SearchAndSetResourceDir
#include "raylib.h"
#include "display/display.h"
#include "include/globals.h"
#include "include/maze.h"
#include "input/input.h"
#include "logic/game_logic.h"
#include "screens/screens.h"
#include "resource_dir.h"
#include <limits.h>
#include "include/notconio.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// Global state for the game loop
static bool should_continue = true;
static bool game_initialized = false;

// Main game update function
void UpdateDrawFrame(void) {
    #ifndef __EMSCRIPTEN__
    if (!game_initialized) {
        init();
        game_initialized = true;
        should_continue = true;
        return;
    }
    #endif

    if (!should_continue) {
        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        close_raylib_display();
        #endif
        return;
    }

    BeginDrawing();

    if (!in_play) {
        int title_result = title_screen();
        
        if (title_result >= 0) {
            init();
            run = true;
            in_play = true;
            printf("loading room %d\n", room);
            load_room();
        }
    } else {
        // Update game state
        update_game();
        
        // Update momentary objects and messages
        update_momentary_object();
        update_message();
        
        // Draw game state
        draw_game();

        // Check game over condition
        if (!in_play) {
            int game_over_result = game_over();
            if (game_over_result >= 0) {
                should_continue = (game_over_result == 1);
            }
        }
    }

    EndDrawing();
}

int main(void) {
    // Initialize Raylib display
    init_raylib_display();
    
    #ifdef __EMSCRIPTEN__
    // Initialize game state
    init();
    game_initialized = true;
    should_continue = true;
    in_play = false;  // Ensure we start at title screen
    
    // Set up the main loop for web
    // Using 0 for fps enables requestAnimationFrame automatically
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
    // Traditional loop for native builds
    SetTargetFPS(60);
    while (!window_should_close()) {
        UpdateDrawFrame();
    }
    
    // Cleanup
    close_raylib_display();
    #endif
    
    return 0;
} 
