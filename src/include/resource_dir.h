#ifndef RESOURCE_DIR_H
#define RESOURCE_DIR_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Resources are now always in the same directory as the executable
#define RESOURCE_PATH "resources/"

// Search for the resources directory and set it as the current working directory
int SearchAndSetResourceDir(const char* resourceDir);

// Helper function to build resource path
void GetResourcePath(const char* filename, char* outPath, size_t outPathSize);

#endif // RESOURCE_DIR_H 