# Raylib Emscripten Dungeon

This is the web-optimized version of the Raylib Dungeon game, specifically designed to run in browsers using Emscripten.

## Key Features

- Web-optimized game loop that works with Emscripten
- Non-blocking input handling for browser compatibility
- Smooth animations and transitions
- Full game functionality preserved from the original version

## Building

To build the web version:

1. Make sure you have Emscripten installed
2. Navigate to the `src` directory
3. Run `./build_web.sh`
4. The compiled files will be in the `web` directory

## Running

To run the web version locally:

1. Navigate to the `web` directory
2. Start a local web server (e.g. `python3 -m http.server 8080`)
3. Open `http://localhost:8080` in your browser

## Controls

- Arrow keys to move
- Space to cast spells
- Enter to confirm selections

## Development Notes

This version has been modified to work with Emscripten's event loop requirements:
- Removed blocking while loops
- Added state management for screen transitions
- Implemented frame-based timing instead of manual delays

## License

Same as the original Raylib Dungeon project. 