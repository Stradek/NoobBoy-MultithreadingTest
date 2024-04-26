# NoobBoy - GameBoy Emulator

NoobBoy is a simple gameboy emulator that I'm writing to learn more about the world of emulation. This emulator has nothing special and has no extra features compared to other DMG emulators.

<img src="screenshots/mario_debug.png" width="800">

## Usage
The emulator was tested on Linux and MacOS. It doesn't work on Windows yet. The SDL2 cmake files are already included, as in some systems cmake can't find the packages.
### Linux and MacOS
To build the project on Linux and MacOS, `cmake`, `SDL2` and `SDL2 ttf` are required.  
### Windows
To build the project on Windows, `cmake`, `SDL2`, `SDL2 ttf`, `SDL2 mixer` and `SDL2 image` are required. 
After downloading binary distributions of `SDL2`, `SDL2 ttf`, `SDL2 mixer` and `SDL2 image` place them in the external directory. `SDL2` `SDL2 ttf`, and `SDL2 image` headers should be placed in `external/include/SDL2`. `SDL2 mixer` headers should be placed in `external/include/SDL2_mixer`. All libraries should be placed in `external/lib`.

### Building
```
$ cmake .
$ make
```
    
### Running
```
usage: ./NoobBoy --rom <rom_file> [--bootrom <bootrom_file>] [--load-save <save_file>] [--debug] [--sound] [--no-bootrom]

arguments:
  --rom                     Standard GameBoy rom
  --bootrom                 Load a custom DMG bootrom
  --debug                   Enable the debugger
  --load-save               Save file to load
  --sound                   Enable experimental sound
  --no-bootrom              Start without bootrom
```


## Input
<table>
<tr><th> Gameplay </th><th>Debug</th></tr>
<tr><td>

| Action | Key |
| --- | --- |
| UP | <kbd>↑</kbd> |
| DOWN | <kbd>↓</kbd> |
| LEFT | <kbd>←</kbd> |
| RIGHT | <kbd>→</kbd> |
| B | <kbd>x</kbd> |
| A | <kbd>z</kbd> |
| START | <kbd>Space</kbd> |
| SELECT | <kbd>Enter</kbd> |
</td><td>

| Action | Key |
| --- | --- |
| Save game | <kbd>s</kbd> |
| Toggle color palette | <kbd>c</kbd> |
| Pause/Resume | <kbd>p</kbd> |
| CPU step | <kbd>n</kbd> |
| Toggle sound | <kbd>m</kbd> |
| Exit | <kbd>Escape</kbd> |

</td></tr> </table>


## Features

- Sound (WIP)
- Correct instructions and PPU timings
- Correct scanline timing
- Sprites with correct row limitations
- Correct sprite, window and background pixel priority
- VRAM rendering with X/Y scroll overlay
- Memory Bank Controller (MBC)
- Debug mode
- Save states (dump of external RAM banks)
- Custom bootrom (Based on [ISSOtm/gb-bootroms](https://github.com/ISSOtm/gb-bootroms/) work)

Currently, the `halt bug` is missing because of some incorrect interrupt timings.


## Game Screenshots


|   |   |
|:---:|:---:|
| ![space-1.jpg](screenshots/cpu_instr.png) |  ![space-1.jpg](screenshots/tetris.png) | 
| *Blargg's CPU Instructions Test* | *Tetris* |
| ![space-1.jpg](screenshots/zelda.png) |  ![space-1.jpg](screenshots/pokemon_blue.png) | 
| *The Legend of Zelda - Link's Awakening* |*Pokemon Blue* |
| ![space-1.jpg](screenshots/dk.png) |  ![space-1.jpg](screenshots/castlevania.png) | 
| *Donkey Kong Land* |*Castlevania* |



**_NOTE:_**  Some games may not work due to some missing MBC features.
