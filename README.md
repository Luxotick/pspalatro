# PSPalatro

This is a limited implementation of the game **Balatro** for **Sony's Playstation Portable** console.  
The project is for legitimate owners of a **Balatro** copy who wish to experience a partial implementation of the game in the PSP.

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/qRNbH5ELMnE/0.jpg)](https://www.youtube.com/watch?v=qRNbH5ELMnE)

## Disclaimer

**This project is not endorsed or affiliated to LocalThunk or Playstack.**
**All rights are reserved to their respective holders.**

You need to own the original game in order to use this software, namely the Windows version (it hasn't been tested with any other version).  
Here's where you can buy it from:
- [Balatro on Steam](https://store.steampowered.com/app/2379780/Balatro/)
- [Balatro on Humble Bundle](https://www.humblebundle.com/store/balatro)
 

## Description 
 
I did this for fun in whatever free time I managed to get.  
There is no roadmap and no plans for future updates, they may come or not, I will try to fix bugs whenever I can.  
Not all implemented rules match the original game exactly, some of them I couldn't find information on (i.e. the probabilities of certain events).

### Features implemented right now:
- Custom background saving system without native PSP UI crashes (by me)
- Main in-game loop
- Shop (singles and boosters)
- Card enhancements
- Card editions
- 80+ jokers
- Consumables (except for spectral cards)
- Full and remaining deck view
- Experimental background music implementation, works ok when overclock is enabled.
- Natural negative jokers

## **Build instructions**

1. Install the PSPSDK (link [here](https://pspdev.github.io/pspsdk/))
2. Check if the following packages are installed for the PSPSDK (using for example psp-pacman):
    - libzip
    - vorbis
    - liblzma
    - stb
3. run "make"

## **Run instructions**

1. Copy EBOOT.PBP and the files in the _assets_ folder to a folder for the game in your PSP.
2. Copy your official Balatro copy executable to that same folder.
3. Modify the _settings.ini_ file (see below for details). _(optional)_

### Configuration (*settings.ini* file)

The _settings.ini_ file can have the following entries:
- archive_file_name (string) - the name of Balatro executable
- hand_size (number) - initial hand size
- hands (number) - initial hands
- discards (number) - initial discards
- wealth (number) - initial wealth ($)
- joker_slots (number) - initial joker slots
- consumable_slots (number) - initial consumable slots
- shop_item_slots (number) - initial shop single slots
- shop_booster_slots (number) - initial shop booster slots
- audio (boolean) - turn audio on or off (it is advisable to use with overclock)
- move_cards (boolean) - turn card oscillation on or off
- overclock (boolean) - set the CPU and BUS clocks to 333Mhz, which I believe it to be fine for the PSP, it may however drain the battery faster
- ante_score_scaling (int) - how the ante score scales (values from 1 to 3)
- speed (int) - how fast scoring is (values from 1 to 5)
