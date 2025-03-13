#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "../include/globals.h"
#include <stdbool.h>

// Game initialization and loop
void init(void);
void game_loop(void);

// Map functions
unsigned char get_map(char x, char y);
void set_map(char x, char y, int tile);
void load_room(void);

// Enemy functions
unsigned int which_enemy(unsigned int ex, unsigned int ey);
void attack(unsigned int weapon, unsigned int ax, unsigned int ay);
void enemy_attack(unsigned int this_enemy);
bool is_within_range(int player_x, int player_y, int enemy_x, int enemy_y, int range);
void move_enemies(void);

// Initialize the game state
void init_game(void);

// Update the game state
void update_game(void);

// Draw the current game state
void draw_game(void);

#endif /* GAME_LOGIC_H */ 