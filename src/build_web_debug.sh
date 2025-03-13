#!/bin/bash
set -e

# Fix Python environment for Emscripten
export PYTHONHOME=/usr
export PYTHONPATH=/usr/lib/python3/dist-packages:/usr/lib/python3.10
export EM_PYTHON=/usr/bin/python3

echo "Building DEBUG web version of Raylib Dungeon..."

# Create web directory if it doesn't exist
mkdir -p ../web_debug

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
    
    # Compile Raylib modules with debug flags
    emcc -c rcore.c -o rcore.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rshapes.c -o rshapes.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rtextures.c -o rtextures.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rtext.c -o rtext.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c rmodels.c -o rmodels.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    emcc -c raudio.c -o raudio.o -O0 -g -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    
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

# Create a simple HTML shell file with extensive debugging
cat > shell_debug.html << EOL
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Raylib Dungeon - DEBUG</title>
    <style>
        body {
            margin: 0;
            background-color: #000;
            color: #fff;
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: flex-start;
            min-height: 100vh;
            padding: 20px;
            overflow-y: auto;
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
            position: fixed;
            top: 10px;
            left: 10px;
            color: #fff;
            background-color: rgba(0, 0, 0, 0.7);
            padding: 5px;
            border-radius: 5px;
            z-index: 100;
            max-width: 300px;
            max-height: 200px;
            overflow-y: auto;
        }
        #error {
            position: fixed;
            top: 10px;
            right: 10px;
            color: #f00;
            background-color: rgba(0, 0, 0, 0.7);
            padding: 5px;
            border-radius: 5px;
            z-index: 100;
            max-width: 300px;
            max-height: 200px;
            overflow-y: auto;
        }
        #console {
            width: 100%;
            max-width: 800px;
            height: 200px;
            background-color: rgba(0, 0, 0, 0.7);
            color: #0f0;
            font-family: monospace;
            padding: 10px;
            margin-top: 20px;
            overflow-y: auto;
            border: 1px solid #333;
        }
        .debug-controls {
            margin-top: 10px;
            display: flex;
            gap: 10px;
        }
        button {
            padding: 5px 10px;
            background-color: #333;
            color: #fff;
            border: none;
            border-radius: 3px;
            cursor: pointer;
        }
        button:hover {
            background-color: #555;
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
    
    <div id="console"></div>
    
    <div class="debug-controls">
        <button id="check-fs">Check FS</button>
        <button id="clear-console">Clear Console</button>
    </div>
    
    <div class="footer">
        <p>Raylib Dungeon - A retro dungeon crawler (DEBUG BUILD)</p>
    </div>
    
    <script>
        // Console logging
        const consoleElement = document.getElementById('console');
        const originalConsoleLog = console.log;
        const originalConsoleError = console.error;
        const originalConsoleWarn = console.warn;
        
        console.log = function() {
            originalConsoleLog.apply(console, arguments);
            const args = Array.from(arguments);
            consoleElement.innerHTML += '<div style="color:#0f0;">[LOG] ' + args.join(' ') + '</div>';
            consoleElement.scrollTop = consoleElement.scrollHeight;
        };
        
        console.error = function() {
            originalConsoleError.apply(console, arguments);
            const args = Array.from(arguments);
            consoleElement.innerHTML += '<div style="color:#f00;">[ERROR] ' + args.join(' ') + '</div>';
            consoleElement.scrollTop = consoleElement.scrollHeight;
            
            // Also update error div
            document.getElementById('error').innerHTML += args.join(' ') + '<br>';
        };
        
        console.warn = function() {
            originalConsoleWarn.apply(console, arguments);
            const args = Array.from(arguments);
            consoleElement.innerHTML += '<div style="color:#ff0;">[WARN] ' + args.join(' ') + '</div>';
            consoleElement.scrollTop = consoleElement.scrollHeight;
        };
        
        // Module configuration
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
            },
            setStatus: function(text) {
                console.log('Status: ' + text);
                document.getElementById('status').innerHTML = text;
            },
            totalDependencies: 0,
            monitorRunDependencies: function(left) {
                this.totalDependencies = Math.max(this.totalDependencies, left);
                Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
            }
        };
        
        // Add error handling
        window.onerror = function(message, source, lineno, colno, error) {
            console.error('Error:', message, source, lineno, colno);
            document.getElementById('error').innerHTML += 'Error: ' + message + '<br>';
        };
        
        // Debug buttons
        document.getElementById('check-fs').addEventListener('click', function() {
            if (Module.FS) {
                console.log('Checking filesystem...');
                try {
                    console.log('Root directory:', Module.FS.readdir('/'));
                    if (Module.FS.readdir('/').includes('resources')) {
                        console.log('Resources directory:', Module.FS.readdir('/resources'));
                    } else {
                        console.error('Resources directory not found!');
                    }
                } catch (e) {
                    console.error('Error reading filesystem:', e);
                }
            } else {
                console.error('FS not available yet');
            }
        });
        
        document.getElementById('clear-console').addEventListener('click', function() {
            consoleElement.innerHTML = '';
        });
    </script>
    {{{ SCRIPT }}}
</body>
</html>
EOL

# Define missing functions that Raylib needs
cat > missing.c << EOL
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <emscripten.h>

void TraceLog(int logLevel, const char *text, ...) {
    va_list args;
    va_start(args, text);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);
    
    // Log to console
    printf("%s\n", buffer);
    
    // Also log to JavaScript console
    EM_ASM({
        console.log(UTF8ToString($0));
    }, buffer);
}

unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead) {
    FILE *file = fopen(fileName, "rb");
    if (!file) { 
        EM_ASM({
            console.error("Failed to open file: " + UTF8ToString($0));
        }, fileName);
        if (bytesRead) *bytesRead = 0; 
        return NULL; 
    }
    
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    EM_ASM({
        console.log("Loading file: " + UTF8ToString($0) + ", size: " + $1);
    }, fileName, size);
    
    unsigned char *data = (unsigned char *)malloc(size);
    if (data) { 
        fread(data, 1, size, file); 
        EM_ASM({
            console.log("File loaded successfully: " + UTF8ToString($0));
        }, fileName);
    } else {
        EM_ASM({
            console.error("Failed to allocate memory for file: " + UTF8ToString($0));
        }, fileName);
    }
    
    fclose(file);
    if (bytesRead) *bytesRead = size;
    return data;
}

void UnloadFileData(unsigned char *data) {
    if (data) {
        free(data);
        EM_ASM({
            console.log("File data unloaded");
        });
    }
}

bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite) {
    FILE *file = fopen(fileName, "wb");
    if (!file) {
        EM_ASM({
            console.error("Failed to open file for writing: " + UTF8ToString($0));
        }, fileName);
        return false;
    }
    
    fwrite(data, 1, bytesToWrite, file);
    fclose(file);
    
    EM_ASM({
        console.log("File saved: " + UTF8ToString($0) + ", size: " + $1);
    }, fileName, bytesToWrite);
    
    return true;
}
EOL

# Compile the missing functions
emcc -c missing.c -o missing.o -O0 -g

# Compile the game with debug flags
echo "Compiling game with Emscripten (DEBUG mode)..."
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
    -o ../web_debug/index.html \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS=ccall,FS \
    -s EXPORTED_FUNCTIONS=_main,_malloc,_free \
    --embed-file ../bin/resources@resources \
    -s ASSERTIONS=2 \
    -s GL_DEBUG=1 \
    -s DEMANGLE_SUPPORT=1 \
    -s SAFE_HEAP=1 \
    -s STACK_OVERFLOW_CHECK=2 \
    -s WARN_UNALIGNED=1 \
    -s EXCEPTION_DEBUG=1 \
    -s DISABLE_EXCEPTION_CATCHING=0 \
    -s RETAIN_COMPILER_SETTINGS=1 \
    -s VERBOSE=1 \
    -g4 \
    -O0 \
    --shell-file shell_debug.html \
    -L../temp -lraylib

# Clean up temporary files
rm -f missing.c missing.o shell_debug.html

echo "Debug build complete. Web files are in ../web_debug/"
echo "You can serve them with a local web server, e.g.:"
echo "  cd ../web_debug && python3 -m http.server 8081"
echo "Then open http://localhost:8081 in your browser." 