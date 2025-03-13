#include "input.h"
#include "../logic/game_logic.h"
#include "../display/display.h"
#include "../include/notconio.h"
#include <time.h>
#include <raylib.h>

// Frame counter for timing
static int frame_counter = 0;

// State for magic spell casting
static struct {
    bool is_casting;
    int fx;
    int fy;
    int remaining_magic;
} spell_state = {false, 0, 0, 0};

int dumb_wait(unsigned int delay) {
    // Convert delay to frames (assuming 60 FPS)
    int frames = (delay / 10000) * 60;
    frame_counter++;
    return frame_counter >= frames;
}

int kbhit(void) {
    return GetKeyPressed() > 0;
}

// Update spell casting state - call this in the main update loop
void update_spell_casting(void) {
    if (!spell_state.is_casting) return;
    
    char c = get_map(spell_state.fx, spell_state.fy);
    if ((c == ' ' || c == '.') && spell_state.remaining_magic > 0) {
        draw_momentary_object(spell_state.fx, spell_state.fy, spell_state.fx, spell_state.fy, 'O', 12);
        spell_state.remaining_magic--;
        spell_state.fx = spell_state.fx + direction_x;
        spell_state.fy = spell_state.fy + direction_y;
    } else {
        // Hit something or ran out of magic
        if (c != ' ' && c != '.') {
            attack(10, spell_state.fx, spell_state.fy);
        }
        spell_state.is_casting = false;
    }
}

unsigned char get_key(void) {
    // Check keys;
    if (IsKeyPressed(KEY_W) && player_y > 0) {
        player_y--;
        return 'w';
    }
    if (IsKeyPressed(KEY_A) && player_x > 0) {
        player_x--;
        return 'a';
    }
    if (IsKeyPressed(KEY_S) && player_y < MAP_HEIGHT-HUD_BOTTOM) {
        player_y++;
        return 's';
    }
    if (IsKeyPressed(KEY_D) && player_x < MAP_WIDTH) {
        player_x++;
        return 'd';
    }
    if ((IsKeyPressed(KEY_O)) && sword) {
        draw_momentary_object(player_x-1, player_y, player_x-1, player_y, '<', 9);
        attack(10, player_x-1, player_y);
        return 'o';
    }
    if ((IsKeyPressed(KEY_P)) && sword) {
        draw_momentary_object(player_x+1, player_y, player_x+1, player_y, '>', 9);
        attack(10, player_x+1, player_y);
        return 'p';
    }
    
    if (IsKeyPressed(KEY_F) && magic > 5 && !spell_state.is_casting) {
        magic -= 5;
        spell_state.fx = player_x + direction_x;
        spell_state.fy = player_y + direction_y;
        spell_state.remaining_magic = magic;
        spell_state.is_casting = true;
        return 'f';
    }
    
    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
        in_play = false;
        return 'Q';
    }
    
    return 0;
} 