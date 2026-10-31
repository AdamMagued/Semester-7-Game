# Lane Defense

A C++17 and OpenGL lane defense game. GLFW, GLAD, GLM, and the image loader are included.

## Build

Use CMake 3.10 or newer, a C++17 compiler, and an OpenGL 4.0 capable graphics driver.

```sh
cmake -S . -B build
cmake --build build --config Release
```

Run from the project folder so the game can find `assets/`.

On macOS or Linux:

```sh
./build/MyGame
```

With the Visual Studio generator on Windows:

```powershell
.\build\Release\MyGame.exe
```

## Controls

- Enter or Start: start a new game.
- Click a lane: select or deselect it. Multiple lanes can be selected.
- Click a card or press 1, 2, or 3: buy melee, ranged, or defense for the last selected lane.
- W or Up: jump with players in selected lanes.
- S or Down: duck with players in selected lanes.
- Escape: pause or continue.
- R: restart from a pause, win, or loss screen.
- M: return to the main menu from a pause, win, or loss screen.

Players move and attack automatically. Defense players stay in place. Defeated enemies drop coins that players collect by touching them. A card is available only when you can afford its price.
