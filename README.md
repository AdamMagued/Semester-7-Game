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
- Click melee/ranged or press 1/2: buy that player for the last selected lane.
- Click defense or press 3, then click a position inside a lane: place a stationary defense player there.
- Ranged and defense cards are available from wave 2 when affordable.
- W or Up: jump with players in selected lanes.
- S or Down: duck with players in selected lanes.
- Escape: pause or continue.
- R: restart from a pause, win, or loss screen.
- M: return to the main menu from a pause, win, or loss screen.
- F11: toggle fullscreen and windowed mode.

Players move and attack automatically. Defense players stay in place. Defeated enemies drop coins that players collect by touching them. A card is available only when you can afford its price.

## Source files

- `src/main.cpp`: window, input, waves, placement, menus and the main loop.
- `src/game/Gameplay.cpp`: movement, attacks, damage and coins.
- `src/game/entities/`: player and enemy data and their basic actions.
- `src/game/Lanes.h`: lane positions shared by drawing and input.
- `src/renderer/Drawing.cpp`: shapes, text, shaders and image loading.
- `src/renderer/Sprites.cpp`: character and spear sprites.
- `src/renderer/Objects.cpp`: coins, bullets and protection indicators.
- `src/ui/Hud.cpp`: player cards and coin count.
- `src/game/conversation_cutscene.cpp`: dialogue and automatic progression.
- `src/game/CutsceneDrawing.cpp`: cutscene characters and dialogue panel.
