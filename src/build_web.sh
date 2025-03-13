#!/bin/bash
set -e

# Fix Python environment for Emscripten
export PYTHONHOME=/usr
export PYTHONPATH=/usr/lib/python3/dist-packages:/usr/lib/python3.10
export EM_PYTHON=/usr/bin/python3

echo "Building web version of Raylib Dungeon..."

# Create web directory if it doesn't exist
mkdir -p ../web

# Create temp directory for building Raylib
mkdir -p ../temp
cd ../temp

# Download and build Raylib for Emscripten
if [ ! -d "raylib" ]; then
    echo "Downloading Raylib..."
    git clone https://github.com/raysan5/raylib.git
    cd raylib
    # Use a specific version known to work with Emscripten
    git checkout 4.5.0
    cd ..
fi

if [ ! -f "libraylib.a" ]; then
    echo "Building Raylib for Emscripten..."
    cd raylib/src
    
    # Check if the platform-specific file exists and patch it if needed
    if [ -f "platforms/rcore_web.c" ]; then
        sed -i 's/glfwSetWindowAttrib/\/\/glfwSetWindowAttrib/g' platforms/rcore_web.c
        sed -i 's/glfwSetWindowContentScaleCallback/\/\/glfwSetWindowContentScaleCallback/g' platforms/rcore_web.c
    fi
    
    # Compile Raylib modules
    emcc -c rcore.c -o rcore.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rshapes.c -o rshapes.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rtextures.c -o rtextures.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rtext.c -o rtext.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rmodels.c -o rmodels.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c raudio.c -o raudio.o -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    
    # Create static library
    emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o
    
    # Copy library and headers to parent directory
    cp libraylib.a ../../
    cp *.h ../../
    
    cd ../../
    echo "Raylib for Emscripten built successfully!"
fi

# Return to the src directory
cd ../src

# Create a simple HTML shell file
cat > shell.html << EOL
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Raylib Dungeon</title>
    <style>
        body {
            margin: 0;
            background-color: #000;
            color: #fff;
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            overflow: hidden;
        }
        .game-container {
            position: relative;
            margin: 0 auto;
        }
        canvas {
            display: block;
            margin: 0 auto;
            border: 1px solid #333;
        }
        .controls {
            margin-top: 20px;
            text-align: center;
        }
        .footer {
            margin-top: 20px;
            font-size: 12px;
            color: #666;
        }
        #status {
            position: absolute;
            top: 10px;
            left: 10px;
            color: #fff;
            background-color: rgba(0, 0, 0, 0.5);
            padding: 5px;
            border-radius: 5px;
            z-index: 100;
        }
        #error {
            position: absolute;
            top: 40px;
            left: 10px;
            color: #f00;
            background-color: rgba(0, 0, 0, 0.5);
            padding: 5px;
            border-radius: 5px;
            z-index: 100;
        }
    </style>
</head>
<body>
    <div id="status">Loading...</div>
    <div id="error"></div>
    
    <div class="game-container">
        <canvas id="canvas" oncontextmenu="event.preventDefault()" tabindex=-1></canvas>
    </div>
    
    <div class="controls">
        <p>Use WASD keys to move, collect treasures, and avoid monsters!</p>
    </div>
    
    <div class="footer">
        <p>Raylib Dungeon - A retro dungeon crawler</p>
    </div>
    
    <script>
        var Module = {
            canvas: (function() { return document.getElementById('canvas'); })(),
            onRuntimeInitialized: function() {
                console.log('WASM runtime initialized');
                document.getElementById('status').innerHTML = 'Running...';
            },
            print: function(text) {
                console.log(text);
                document.getElementById('status').innerHTML = text;
            },
            printErr: function(text) {
                console.error(text);
                document.getElementById('error').innerHTML += text + '<br>';
            },
            setStatus: function(text) {
                console.log(text);
                document.getElementById('status').innerHTML = text;
            }
        };
        
        // Add error handling
        window.onerror = function(message, source, lineno, colno, error) {
            console.error('Error:', message, source, lineno, colno, error);
            document.getElementById('error').innerHTML += 'Error: ' + message + '<br>';
        };
    </script>
    {{{ SCRIPT }}}
</body>
</html>
EOL

# Compile with Emscripten
echo "Compiling game with Emscripten..."

# Define missing functions that Raylib needs
cat > missing.c << EOL
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void TraceLog(int logLevel, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vprintf(text, args);
    va_end(args);
}

unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead) {
    FILE *file = fopen(fileName, "rb");
    if (!file) { if (bytesRead) *bytesRead = 0; return NULL; }
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char *data = (unsigned char *)malloc(size);
    if (data) { fread(data, 1, size, file); }
    fclose(file);
    if (bytesRead) *bytesRead = size;
    return data;
}

void UnloadFileData(unsigned char *data) {
    if (data) free(data);
}

bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite) {
    FILE *file = fopen(fileName, "wb");
    if (!file) return false;
    fwrite(data, 1, bytesToWrite, file);
    fclose(file);
    return true;
}
EOL

# Compile the missing functions
emcc -c missing.c -o missing.o

# Compile the game
emcc -Wall -Wextra -I../temp -Iinclude \
    main.c \
    display/display.c \
    include/globals.c \
    include/maze.c \
    input/input.c \
    logic/game_logic.c \
    screens/screens.c \
    include/notconio.c \
    include/resource_dir.c \
    missing.o \
    -o ../web/index.html \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS=ccall \
    -s EXPORTED_FUNCTIONS=_main,_malloc,_free \
    --embed-file ../bin/resources@resources \
    -s ASSERTIONS=1 \
    -s GL_DEBUG=1 \
    -s DEMANGLE_SUPPORT=1 \
    -s SAFE_HEAP=1 \
    -s STACK_OVERFLOW_CHECK=1 \
    --shell-file shell.html \
    -L../temp -lraylib

# Clean up temporary files
rm -f missing.c missing.o shell.html

echo "Build complete. Web files are in ../web/"
echo "You can serve them with a local web server, e.g.:"
echo "  cd ../web && python3 -m http.server 8080"
echo "Then open http://localhost:8080 in your browser." 