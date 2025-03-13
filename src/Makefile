CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Detect OS
UNAME_S := $(shell uname -s)

# macOS specific flags
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -I/opt/homebrew/include
    LDFLAGS = -L/opt/homebrew/lib -lraylib -framework OpenGL -framework IOKit -framework Cocoa
endif

# Linux specific flags
ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Emscripten compiler and flags
EMCC = emcc
EMCFLAGS = -Wall -Wextra -Iinclude
EMLDFLAGS = -s USE_GLFW=3 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s EXPORTED_RUNTIME_METHODS=ccall \
            -s EXPORTED_FUNCTIONS=_main,_malloc,_free --preload-file ../bin/resources@/resources --shell-file shell.html

# Source files
SRCS = main.c \
       display/display.c \
       include/globals.c \
       include/maze.c \
       input/input.c \
       logic/game_logic.c \
       screens/screens.c \
       include/notconio.c \
       include/resource_dir.c

# Object files
OBJS = $(SRCS:.c=.o)

# Targets
TARGET = retro-c-raylib
BINDIR = ../bin
WEBDIR = ../web

# Default target
all: $(TARGET) install

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Install the executable to the bin directory
install: $(TARGET)
	mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)/

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Web target using Emscripten
web: shell.html
	mkdir -p $(WEBDIR)
	$(EMCC) $(EMCFLAGS) $(SRCS) -o $(WEBDIR)/index.html $(EMLDFLAGS)

# Create a basic shell HTML file if it doesn't exist
shell.html:
	@echo "Creating shell.html template..."
	@echo "<!DOCTYPE html>" > shell.html
	@echo "<html lang=\"en\">" >> shell.html
	@echo "<head>" >> shell.html
	@echo "    <meta charset=\"UTF-8\">" >> shell.html
	@echo "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" >> shell.html
	@echo "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" >> shell.html
	@echo "    <title>Raylib Dungeon</title>" >> shell.html
	@echo "    <style>" >> shell.html
	@echo "        body { margin: 0; background-color: black; }" >> shell.html
	@echo "        .emscripten { position: absolute; top: 0; left: 0; margin: 0; border: 0; width: 100%; height: 100%; overflow: hidden; display: block; }" >> shell.html
	@echo "        canvas { display: block; margin: auto; }" >> shell.html
	@echo "    </style>" >> shell.html
	@echo "</head>" >> shell.html
	@echo "<body>" >> shell.html
	@echo "    <canvas class=\"emscripten\" id=\"canvas\" oncontextmenu=\"event.preventDefault()\" tabindex=-1></canvas>" >> shell.html
	@echo "    <script>" >> shell.html
	@echo "        var Module = {" >> shell.html
	@echo "            canvas: (function() { return document.getElementById('canvas'); })()," >> shell.html
	@echo "            onRuntimeInitialized: function() {" >> shell.html
	@echo "                console.log('WASM runtime initialized');" >> shell.html
	@echo "            }" >> shell.html
	@echo "        };" >> shell.html
	@echo "    </script>" >> shell.html
	@echo "    {{{ SCRIPT }}}" >> shell.html
	@echo "</body>" >> shell.html
	@echo "</html>" >> shell.html

# Clean
clean:
	rm -f $(OBJS) $(TARGET) $(BINDIR)/$(TARGET) shell.html
	rm -rf $(WEBDIR)

.PHONY: all clean install web 