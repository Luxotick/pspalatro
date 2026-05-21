# PSPalatro

**Work in progress PSP implementation of Balatro.**

This project is still under active development and is open to the community.  
Feedback, bug reports, suggestions and contributions are welcome.

This repository is based on a fork of the original PSPalatro project by [internalregister](https://github.com/internalregister), with additional updates, fixes and features added by contributors in this version.

This is a limited implementation of the game **Balatro** for **Sony's Playstation Portable** console.  
The project is for legitimate owners of a **Balatro** copy who wish to experience a partial implementation of the game on the PSP.

## Disclaimer

**This project is not endorsed or affiliated with LocalThunk or Playstack.**  
**All rights are reserved to their respective holders.**

You need to own the original game in order to use this software, namely the Windows version.  
It has not been tested with other versions.

Here's where you can buy it from:

- [Balatro on Steam](https://store.steampowered.com/app/2379780/Balatro/)
- [Balatro on Humble Bundle](https://www.humblebundle.com/store/balatro)

## Description

I did this for fun in whatever free time I managed to get.

The project is still being improved and may receive new updates over time.  
There is no strict roadmap, but I will try to fix bugs and add improvements whenever I can.

Not all implemented rules match the original game exactly. Some of them were adjusted due to PSP limitations, missing information, or implementation differences.

### Features implemented right now:

- Native PSP save system
- Pause menu with runtime settings
- Main in-game loop
- Shop, including singles and boosters
- Card enhancements
- Card editions
- 80+ jokers
- Consumables
- Spectral cards and Spectral booster packs
- Full and remaining deck view
- Experimental background music implementation, works best when overclock is enabled
- Sound effects for UI, scoring, shop and card actions
- Save-safe audio callback handling for native PSP save dialogs
- Natural negative jokers

### Currently being worked on / planned:

- Bug fixes and stability improvements
- More gameplay polishing
- Additional updates based on feedback and available time

## Special Thanks

Special thanks to [internalregister](https://github.com/internalregister), the original author of the PSPalatro port.

Huge thanks to [OniMock](https://github.com/OniMock) for the major help, support and contributions to this version of the project.

Thanks to [luxotick](https://github.com/luxotick) for ongoing updates, fixes and PSP integration work.

## Build instructions

1. Install the PSPSDK: [PSPSDK](https://pspdev.github.io/pspsdk/)
2. Check if the following packages are installed for the PSPSDK, using for example `psp-pacman`:
   - libzip
   - vorbis
   - liblzma
   - stb
3. Run:

```sh
make
```

## Run instructions

1. Copy `EBOOT.PBP` and the files in the `assets` folder to a folder for the game on your PSP.
2. Copy your official Balatro executable to that same folder.
3. Modify the `settings.ini` file if needed. This step is optional.

The final folder on your PSP should look like this:

```text
ms0:/PSP/GAME/PSPalatro/
├── EBOOT.PBP
├── Balatro.exe
├── settings.ini
└── editions.png
```

The files inside the repository `assets` folder should be copied next to `EBOOT.PBP`, not kept inside an `assets` folder on the PSP.

If you rename the Balatro executable, update `archive_file_name` in `settings.ini` to match the new filename.

## Configuration

The `settings.ini` file can have the following entries:

- `archive_file_name` - string - the name of the Balatro executable
- `hand_size` - number - initial hand size
- `hands` - number - initial hands
- `discards` - number - initial discards
- `wealth` - number - initial wealth
- `joker_slots` - number - initial joker slots
- `consumable_slots` - number - initial consumable slots
- `shop_item_slots` - number - initial shop single slots
- `shop_booster_slots` - number - initial shop booster slots
- `audio` - boolean - turn audio on or off. It is advisable to use this with overclock enabled
- `move_cards` - boolean - turn card oscillation on or off
- `debug_tools` - boolean - enable or disable runtime debug shortcuts
- `overclock` - boolean - set the CPU and BUS clocks to 333MHz. This should be fine for the PSP, but it may drain the battery faster
- `ante_score_scaling` - int - how the ante score scales, values from 1 to 3
- `speed` - int - how fast scoring is, values from 1 to 5

## Debug Mode Keybindings

To use these bindings, **Debug Tools** (`debug_tools`) must first be enabled either through the in-game settings menu or by setting `debug_tools = true` in your `settings.ini`. Almost all debug commands require you to hold down both the **Left Trigger (L)** and **Right Trigger (R)** on the PSP while pressing a face button.

### 1. Modifying Game Progression (Deck Select Menu)
**Condition to use:** You must be on the Title Screen, currently focused on the **Deck Selection** menu.
*   **`L` + `R` + `Cross (X)`**: Unlocks the next deck tier.
    *   *When to use:* Use this when you are testing gameplay with advanced deck types but don't want to play through the standard unlock progression.
*   **`L` + `R` + `Circle (O)`**: Locks all deck progress and resets you to the default state (Red Deck).
    *   *When to use:* Use this to test the "fresh save file" experience or to verify if deck unlock conditions are working properly.

### 2. Modifying Individual Cards (In-Game)
**Condition to use:** You must be actively in a match, picking your hand, and highlighting a specific card in your active hand.
*   **`L` + `R` + `Square`**: Cycles through the available **Card Seals** (e.g., Red, Blue, Gold, Purple) for the highlighted card.
    *   *When to use:* Use this to easily test seal-specific effects or interactions without relying on random shop RNG or Tarot/Spectral cards.
*   **`L` + `R` + `Circle (O)`**: Removes the **Card Seal** from the highlighted card entirely.
    *   *When to use:* Use this to revert a card to its base state while testing seal modifiers.
*   **`L` + `R` + `Cross (X)`**: Cycles through the different **Card Editions** (e.g., Foil, Holographic, Polychrome).
    *   *When to use:* Use this to quickly buff a hand to test scoring capabilities, or to ensure that special edition shaders (like Foil or Holographic) are rendering properly on the hardware.

### 3. Visual & UI Testing (Anywhere)
**Condition to use:** Can be triggered at any time during gameplay or menus.
*   **`L` + `R` + `Triangle`**: Toggles **"Force Score Flames"**.
    *   *When to use:* This forces the game to display the intense flame visual effects on the score. It is primarily used for debugging the visual rendering (e.g., shaders, particles) or testing the PSP's performance/frame rate under high visual stress.

