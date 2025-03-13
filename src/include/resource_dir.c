#include "resource_dir.h"

int SearchAndSetResourceDir(const char* resourceDir) {
    char currentPath[PATH_MAX];
    char testPath[PATH_MAX];
    
    // Get the current working directory
    if (getcwd(currentPath, sizeof(currentPath)) == NULL) {
        printf("Error getting current directory\n");
        return 0;
    }

    printf("Current working directory: %s\n", currentPath);

    // For Emscripten builds, resources are preloaded at the root
    #ifdef __EMSCRIPTEN__
    printf("Emscripten build detected, resources should be at: %s\n", resourceDir);
    return 1;
    #else
    // Try the current directory first
    snprintf(testPath, sizeof(testPath), "%s/%s", currentPath, resourceDir);
    printf("Trying resource path: %s\n", testPath);
    
    if (access(testPath, F_OK) == 0) {
        printf("Found resources at: %s\n", testPath);
        return 1;
    }

    printf("Error: Could not find resources directory at %s\n", testPath);
    return 0;
    #endif
}

void GetResourcePath(const char* filename, char* outPath, size_t outPathSize) {
    #ifdef __EMSCRIPTEN__
    // In Emscripten, resources are preloaded at the root of the virtual filesystem
    snprintf(outPath, outPathSize, "resources/%s", filename);
    #else
    snprintf(outPath, outPathSize, "%s%s", RESOURCE_PATH, filename);
    #endif
    printf("Loading resource: %s\n", outPath);
} 