/* 
    Duplicate or similar to CONIO functions
*/

#ifndef NOTCONIO_H
#define NOTCONIO_H

#include <stdbool.h>
#include <string.h>
#include "globals.h"
#include "../display/display.h"

// Text mode compatibility functions
void clrscr(void);
void cputsxy(unsigned int x, unsigned int y, const char* str);
void cputcxy(unsigned int x, unsigned int y, char c);
void gotoxy(unsigned int x, unsigned int y);
void cursor(int show);
int cgetc(void);
void refresh(void);
void echo(void);

// Display functions
void output_message(void);
void draw_screen(void);
void draw_char(int x, int y, unsigned char c);
void update_message(void);

// Input functions
void nodelay(void* win, int val);

// Momentary object functions
void draw_momentary_object(unsigned int obj_old_x, unsigned int obj_old_y, 
                          unsigned int obj_x, unsigned int obj_y, 
                          unsigned int obj_tile, int delay);
void update_momentary_object(void);

#endif /* NOTCONIO_H */ 