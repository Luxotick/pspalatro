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
