#include "screens.h"
#include "../include/globals.h"
#include "../include/maze.h"
#include "../input/input.h"
#include "../logic/game_logic.h"
#include "../display/display.h"
#include <raylib.h>
#include "../include/notconio.h"
#include <string.h>

unsigned char show_titles;
static int title_counter = 0;

unsigned char logo[] = {
    46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
    35, 35, 35, 35, 46, 46, 35, 46, 46, 35, 46, 35, 46, 46, 46, 35, 46, 46, 35, 35, 35, 35, 46, 46, 35, 35, 35, 35, 46, 46, 35, 35, 35, 46, 46, 35, 46, 46, 46, 35,
    46, 35, 46, 46, 35, 46, 35, 46, 46, 35, 46, 35, 35, 46, 46, 35, 46, 35, 46, 46, 46, 46, 35, 46, 35, 46, 46, 46, 46, 35, 46, 46, 46, 35, 46, 35, 35, 46, 46, 35,
    46, 35, 46, 46, 35, 46, 35, 46, 46, 35, 46, 35, 46, 35, 46, 35, 46, 35, 46, 46, 46, 46, 46, 46, 35, 35, 46, 46, 46, 35, 46, 46, 46, 35, 46, 35, 46, 35, 46, 35,
    46, 35, 46, 46, 35, 46, 35, 46, 46, 35, 46, 35, 46, 46, 35, 35, 46, 35, 46, 35, 35, 35, 35, 46, 35, 46, 46, 46, 46, 35, 46, 46, 46, 35, 46, 35, 46, 46, 35, 35,
    46, 35, 46, 46, 35, 46, 35, 46, 46, 35, 46, 35, 46, 46, 46, 35, 46, 35, 46, 46, 46, 46, 35, 46, 35, 46, 46, 46, 46, 35, 46, 46, 46, 35, 46, 35, 46, 46, 46, 35,
    35, 35, 35, 35, 46, 46, 46, 35, 35, 46, 46, 35, 46, 46, 46, 35, 46, 46, 35, 35, 35, 35, 46, 46, 35, 35, 35, 35, 46, 46, 35, 35, 35, 46, 46, 35, 46, 46, 46, 35,
    46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46
};

void center_text(unsigned char row, char *text) {
    // Calculate the position to center the text
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    int x = (SCREEN_WIDTH - textWidth) / 2;
    int y = row * 24; // Spacing between lines
    
    // Draw the text using raylib
    DrawText(text, x, y, fontSize, WHITE);
}

void display_titles(void) {
    // Show the word DUNGEON using the custom character set
    int row;
    
    // Output the logo line by line
    for (row = 0; row < 8; row++) {
        for (int col = 0; col < 40; col++) {
            char c = (logo[row * 40 + col] == 35) ? '#' : '.';
            draw_char(col, row, c);
        }
    }

    int fontSize = 20;
    DrawText("The Dungeon", (SCREEN_WIDTH - MeasureText("The Dungeon", 30)) / 2, 10 * 24, 30, WHITE);
    DrawText("a game by retrogamecoders.com", (SCREEN_WIDTH - MeasureText("a game by retrogamecoders.com", fontSize)) / 2, 15 * 24, fontSize, WHITE);
    DrawText("press a key", (SCREEN_WIDTH - MeasureText("press a key", fontSize)) / 2, 20 * 24, fontSize, WHITE);
}

void display_instructions(void) {
    int fontSize = 20;
    int titleSize = 30;
    
    DrawText("Instructions", (SCREEN_WIDTH - MeasureText("Instructions", titleSize)) / 2, 6 * 24, titleSize, WHITE);
    DrawText("use the wasd keys to move", (SCREEN_WIDTH - MeasureText("use the wasd keys to move", fontSize)) / 2, 12 * 24, fontSize, WHITE);
    DrawText("collect the treasure", (SCREEN_WIDTH - MeasureText("collect the treasure", fontSize)) / 2, 14 * 24, fontSize, WHITE);
    DrawText("avoid the monsters", (SCREEN_WIDTH - MeasureText("avoid the monsters", fontSize)) / 2, 16 * 24, fontSize, WHITE);
    DrawText("press a key to start", (SCREEN_WIDTH - MeasureText("press a key to start", fontSize)) / 2, 20 * 24, fontSize, WHITE);
}

// This is the default title screen
int title_screen(void) {
    static bool title_initialized = false;
    
    if (!title_initialized) {
        title_counter = 0;
        show_titles = 1;
        clrscr();
        title_initialized = true;
    }
    
    // Switch display every 500 ticks
    if (title_counter >= 250) {
        title_counter = 0;
        show_titles = !show_titles;
    } else {
        title_counter++;
    }
    
    if (show_titles) {
        display_titles();
    } else {
        display_instructions();
    }
    
    if (GetCharPressed()) {
        title_initialized = false;
        in_play = true;
        clrscr();
        return title_counter;
    }
    
    return -1;  // Return -1 to indicate we're still in the title screen
}

// Return values:
// -1: Still in game over screen
//  0: User chose to quit
//  1: User chose to play again
int game_over(void) {
    static bool game_over_initialized = false;
    
    if (!game_over_initialized) {
        clrscr();
        in_play = false;
        game_over_initialized = true;
    }
    
    BeginDrawing();
    clrscr();
    
    int fontSize = 20;
    int titleSize = 30;
    
    DrawText("Game Over", (SCREEN_WIDTH - MeasureText("Game Over", titleSize)) / 2, 10 * 24, titleSize, WHITE);
    DrawText("ah, such a shame,", (SCREEN_WIDTH - MeasureText("ah, such a shame,", fontSize)) / 2, 12 * 24, fontSize, WHITE);
    DrawText("you were doing so well!", (SCREEN_WIDTH - MeasureText("you were doing so well!", fontSize)) / 2, 14 * 24, fontSize, WHITE);

    sprintf(output, "score:%03d", score);
    DrawText(output, (SCREEN_WIDTH - MeasureText(output, fontSize)) / 2, 18 * 24, fontSize, WHITE);
    DrawText("play again (y/n)?", (SCREEN_WIDTH - MeasureText("play again (y/n)?", fontSize)) / 2, 19 * 24, fontSize, WHITE);
    
    EndDrawing();
    
    // Check for key press
    int key = GetCharPressed();
    if (key == 'y' || key == 'Y') {
        game_over_initialized = false;
        init();
        return 1;
    } else if (key == 'n' || key == 'N') {
        game_over_initialized = false;
        return 0;
    }
    
    return -1; // Still in game over screen
} 