#include "game_logic.h"
#include "../display/display.h"
#include "../input/input.h"
#include "../include/maze.h"
#include "../include/notconio.h"
#include <time.h>
#include "raylib.h"

void draw_hud(void);

// (re)initialize game
void init(void) {
    // Initialize player stats
    health = 100;
    score = 0;
    keys = 0;
    room = 1;
    potion = 0;
    magic = 0;
    sword = false;
    weapon = 1;
    idols = 0;

    // Initialize game state
    enemy_count = 0;
    draw_whole_screen = false;
    screen_drawn = false;
    in_play = true;
    run = true;
    obstruction = false;

    // Clear visibility map
    memset(visibility_map, 0, sizeof(visibility_map));
}

unsigned char get_map(char x, char y) {
    unsigned char c;
    c = game_map[MAP_WIDTH*y+x];
    if (c == 0) {
        return '.';
    }
    return c;
}

void set_map(char x, char y, int tile) {
    // Set the part of the array to the given tile
    game_map[MAP_WIDTH*y+x] = tile;
}


void load_room(void) {
    int pos = 0;
    player_x = -1;
    player_y = -1;
    old_x = -1;
    old_y = -1;
    draw_whole_screen = false;
    screen_drawn = false;

    // Initialize visibility map to all unseen
    memset(visibility_map, 0, sizeof(visibility_map));

    clrscr();
    
    sprintf(output, "loading room %d", room);
    output_message();

    srand((unsigned)time(NULL));
    carveMaze();
    placePlayer();

    // 1 Gobbo less than per room level
    for (i = 0; i < room-1; i++) placeObject('g');
    
    // 1 piece of idol per room level
    for (i = 0; i < room; i++) placeObject('i');
    
    // Increase rats per room level   
    for (i = 0; i < room+1; i++) placeObject('r');

    placeObject('*');
    placeObject('|');
    placeObject('$');
    placeObject('h');
    placeObject('k');
    placeObject('s'); // spell
    placeHDoor();
    placeVDoor();

    /* Copy the generated maze into the full map.
       First fill the entire map with walls. */
    for (this_row = 0; this_row < PLAYABLE_HEIGHT; this_row++) {
        for (this_col = 0; this_col < MAP_WIDTH; this_col++) {
            game_map[pos] = '#';   
            pos++;  
        }
    }

    pos = 0;
    for (this_row = 0; this_row < PLAYABLE_HEIGHT; this_row++) {  
        for (this_col = 0; this_col < MAP_WIDTH; this_col++) { 
            c = map[this_row][this_col];

            // Player x and y
            if (c == '@') {
                player_y = this_row;
                player_x = this_col;
                old_y = this_row;
                old_x = this_col;
                visibility_map[MAP_WIDTH * old_y + old_x] = false;
            }

            // Goblin
            if (c == 'g') {
                // Increment for next enemy (Enemy 0 is counted as no enemy)
                enemy_count += 1;

                // Create the enemy in the list
                enemies[enemy_count].tile = c;
                enemies[enemy_count].room = room;
                enemies[enemy_count].x = this_col;
                enemies[enemy_count].y = this_row;
                enemies[enemy_count].old_x = enemies[enemy_count].x;
                enemies[enemy_count].old_y = enemies[enemy_count].y;
                enemies[enemy_count].health = 30;
                enemies[enemy_count].strength = 5;
                enemies[enemy_count].speed = 1;
                enemies[enemy_count].armour = 10;
            }  
            // Rat
            else if (c == 'r') {
                // Increment for next enemy (Enemy 0 is counted as no enemy)
                enemy_count += 1;

                // Create the enemy in the list
                enemies[enemy_count].tile = c;
                enemies[enemy_count].room = room;
                enemies[enemy_count].x = this_col;
                enemies[enemy_count].y = this_row;
                enemies[enemy_count].old_x = enemies[enemy_count].x;
                enemies[enemy_count].old_y = enemies[enemy_count].y;
                enemies[enemy_count].health = 15;
                enemies[enemy_count].strength = 5;
                enemies[enemy_count].speed = 2;
                enemies[enemy_count].armour = 0;
            }  
            game_map[pos] = c;   
            pos++;  
        }
    }
}


// Returns the enemy for a given x,y coord
unsigned int which_enemy(unsigned int ex, unsigned int ey) {
    if (get_map(ex, ey) == ' ' || get_map(ex, ey) == '.') return 0;

    // Enemies starts at 1, 0 = no enemy
    for (i = 1; i < enemy_count+1; i++) {
        if (enemies[i].x == ex && enemies[i].y == ey && enemies[i].health >= 1) return i;
    }

    // No enemies
    return 0;
}

void attack(unsigned int weapon, unsigned int ax, unsigned int ay) {
    int rnum = 0;
    this_enemy = 0;
    this_enemy = which_enemy(ax, ay);
    if (this_enemy == 0) {
        return;
    } 

    rnum = (rand() % (20 - 1 + 1)) + 1; 

    if (rnum > enemies[this_enemy].armour + enemies[this_enemy].speed) {
        // Damage!
        enemies[this_enemy].health -= weapon;
        if (enemies[this_enemy].health > 0) {
            sprintf(output, "hit!! enemy health: %3d    ", enemies[this_enemy].health);
            output_message();
            
        }
    } else {
        sprintf(output, "miss! enemy health: %3d ", enemies[this_enemy].health);
        output_message();

        if ((player_x == ax && player_y == ay) || 
            (player_x == ax && (player_y == ay + 1 || player_y == ay - 1)) || 
            (player_y == ay && (player_x == ax + 1 || player_x == ax - 1))) {
            health -= enemies[this_enemy].strength;
        }
        
    }

    if (enemies[this_enemy].health < 1) {
        // Success!
        sprintf(output, "enemy defeated!");
        output_message();
        
        // Draw tile in new location
        cputcxy(ax, ay, '.'); 
        set_map(ax, ay, '.');
        enemies[this_enemy].tile = '.';

        // Up the score
        score += 10;
        
    }
}

void enemy_attack(unsigned int this_enemy) {
    int rnum = 0;
    rnum = (rand() % (20 - 1 + 1)) + 1; 

    if (rnum > 10) {
        // Damage!
        if (health < 1 || (health-enemies[this_enemy].strength) < 1) {
            health = 0;
        } else {
            health -= enemies[this_enemy].strength;
        }    
        
        sprintf(output, "attacked! health: %3d", health);
        output_message();
    } else {
        enemies[this_enemy].health -= 5;
        if (enemies[this_enemy].health < 1) {
            enemies[this_enemy].health = 0;
            // Draw tile in new location
            cputcxy(enemies[this_enemy].x, enemies[this_enemy].y, '.'); 
            set_map(enemies[this_enemy].x, enemies[this_enemy].y, '.');
            enemies[this_enemy].tile = '.';
            
            sprintf(output, "enemy defeated!");
            output_message();
        } else {
            sprintf(output, "block health: %3d", health);
            output_message();
        }
    }

    if (health < 1) {
        // Fail!
        sprintf(output, "enemy defeated you!");
        output_message();
        health = 0;
    }
}

// How close is the player?
bool is_within_range(int player_x, int player_y, int enemy_x, int enemy_y, int range) {
    int dx = enemy_x - player_x;
    int dy = enemy_y - player_y;
    return (dx * dx + dy * dy) <= (range * range);
}




// Move the enemies for a given room
void move_enemies(void) {
    unsigned char rnd;

    // Enemies starts at 1, 0 = no enemy
    for (i = 1; i < enemy_count+1; i++) {
        if (enemies[i].room == room && enemies[i].health > 0) {
            enemies[i].old_x = enemies[i].x;
            enemies[i].old_y = enemies[i].y; 
            
            // Rat is random
            if (enemies[i].tile == 'r' && is_within_range(player_x, player_y, enemies[i].x, enemies[i].y, 6)) {
                rnd = (rand() % (4)) + 1; 
                if (rnd == 4) enemies[i].x -= 1;
                if (rnd == 2) enemies[i].x += 1;
                if (rnd == 1) enemies[i].y -= 1;
                if (rnd == 3) enemies[i].y += 1;
            }

            // Gobbo goes for player
            if (enemies[i].tile == 'g' && is_within_range(player_x, player_y, enemies[i].x, enemies[i].y, 6)) {
                if (enemies[i].x > player_x) enemies[i].x -= 1;
                if (enemies[i].x < player_x) enemies[i].x += 1;
                if (enemies[i].y > player_y) enemies[i].y -= 1;
                if (enemies[i].y < player_y) enemies[i].y += 1;
            }




            // Redraw
            c = get_map(enemies[i].x, enemies[i].y);
            
            if ((c != ' ' && c != '.') || c == enemies[i].tile) {
                enemies[i].x = enemies[i].old_x;
                enemies[i].y = enemies[i].old_y;
                set_map(enemies[i].x, enemies[i].y, enemies[i].tile);

                // Attack if adjacent to player
                if (c=='@') enemy_attack(i);

            } else {
                set_map(enemies[i].old_x, enemies[i].old_y, '.');
                set_map(enemies[i].x, enemies[i].y, enemies[i].tile);
            }


            //cputcxy(enemies[i].x, enemies[i].y, enemies[i].tile);


        }
    }
}


void game_loop(void) {
    key = 0;

    // Backup the location
    old_x = player_x;
    old_y = player_y;

    // Get player input first
    key = get_key();

    // Change direction
    if (player_x != old_x || player_y != old_y) {
        direction_x = player_x - old_x;
        direction_y = player_y - old_y;
    }
    
    // Only process movement and enemies if player made a move
    if (key != 0) {
        // Process player movement and collisions
        obstruction = false;
        c = get_map(player_x, player_y);
        
        // Process collisions and interactions
        switch (c) {
            case '#':
                // Wall               
                obstruction = true;
                break;
            
            // Spell
            case 's':
                draw_whole_screen = true;
                sprintf(output, "you activated a spell");
                break;

            case 'k': // Key +1
                keys += 1;
                sprintf(output, "you found a key");
                break;

            // Door
            case '+':
                if (keys > 0) {
                    keys -= 1;
                    score += 5;
                    obstruction = false;
                    sprintf(output, "you opened a door");
                } else {
                    // Not enough keys to unlock!
                    set_map(player_x, player_y, '-'); // turn into partially open
                    health -= 10; // lose 10 health
                    obstruction = true;
                    sprintf(output, "ouch!");
                }
                break;

            case '-': // Partially open door
                if (keys > 0) {
                    keys -= 1;
                    score += 5;
                    obstruction = false;
                    sprintf(output, "you opened a door");
                } else {
                    // Not enough keys to unlock!
                    set_map(player_x, player_y, '.');  // turn into fully open
                    health -= 10;         // lose 10 health
                    obstruction = true;
                    sprintf(output, "who needs keys anyway?");
                }
                break;

            case '|': // Sword!
                sword = true;
                
                if (weapon < 5) {
                    weapon = 5;
                    sprintf(output, "you found a sword!");
                } else {
                    weapon++;
                    sprintf(output, "+1 to your attack!");
                }
                break;

            case '$': // Cash money
                score += 50;
                sprintf(output, "ka-ching!");
                break;

            case '*': // Potion
                score += 15;
                magic += 100;
                sprintf(output, "power up!");
                break;

            case 'm': // Cash money
                score += 15;
                break;

            case 'h': // Health
                health += 25;
                sprintf(output, "ahh that is better!");
                if (health > 100) health = 100; // Can't be more than 100%!
                break;

            /* Enemies >> */
            case 'g': // Gobbo
                attack(weapon, player_x, player_y);
                obstruction = true;
                break;

            case 'r': // Rats
                attack(weapon, player_x, player_y);
                obstruction = true;
                break;
            /* ^^ Enemies */

            case 'i': // Idol
                score += 10;
                idols += 1;
                if (idols == room) {
                    room += 1;
                    load_room();
                    draw_screen();
                    idols = 0;
                }
                break;

            
            default:
                set_map(old_x, old_y, '.');
                set_map(player_x, player_y, '@');
            
                break;
        }

        if (output[1] > 32) {
            output_message();
        }

        // If obstructed then bounce
        if (obstruction) {
            player_x = old_x;
            player_y = old_y;
            set_map(player_x, player_y, '@');
        } else {
            set_map(old_x, old_y, '.');
            set_map(player_x, player_y, '@');
            move_enemies();
        }

        if (health < 1) {
            in_play = false;
        }
    }
}

void init_game(void) {
    init();  // Call the original init function
}

void update_game(void) {
    if (in_play) {
        game_loop();
        update_spell_casting();  // Update spell casting state
    }
}

void draw_game(void) {
    BeginDrawing();
    clear_screen();
    draw_screen();
    draw_hud();
    EndDrawing();
}

void draw_hud(void) {
    char hud_text[40];
    
    int fontSize = 20;
    int iconSize = 20; // Assuming the textures are 20x20 pixels
    int padding = 5; // Space between icon and text
    int startX = (SCREEN_WIDTH - (3 * iconSize + 3 * padding + MeasureText(":00 :000 :000 score:0000", fontSize))) / 2;
    int y = SCREEN_HEIGHT - fontSize - 10; // 10 pixels from the bottom


    //sprintf(hud_text, "%02d,%02d", player_x, player_y);
    //DrawText(hud_text, 0, 0, fontSize, WHITE);

    // Draw key icon and text
    DrawTexture(textures['k'], startX, y, WHITE);
    sprintf(hud_text, ":%02d", keys);
    DrawText(hud_text, startX + iconSize + padding, y, fontSize, WHITE);

    // Draw health icon and text
    int healthTextStartX = startX + iconSize + padding + MeasureText(hud_text, fontSize) + padding;
    DrawTexture(textures['h'], healthTextStartX, y, WHITE);
    sprintf(hud_text, ":%03d", health);
    DrawText(hud_text, healthTextStartX + iconSize + padding, y, fontSize, WHITE);

    // Draw magic icon and text
    int magicTextStartX = healthTextStartX + iconSize + padding + MeasureText(hud_text, fontSize) + padding;
    DrawTexture(textures['*'], magicTextStartX, y, WHITE);
    sprintf(hud_text, ":%03d score:%04d", magic, score);
    DrawText(hud_text, magicTextStartX + iconSize + padding, y, fontSize, WHITE);
} 