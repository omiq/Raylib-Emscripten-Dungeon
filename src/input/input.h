#ifndef INPUT_H
#define INPUT_H

#include "../include/globals.h"

// Input functions
unsigned char get_key(void);
int dumb_wait(unsigned int delay);
int kbhit(void);
void update_spell_casting(void);  // Add spell casting update function

#endif /* INPUT_H */ 