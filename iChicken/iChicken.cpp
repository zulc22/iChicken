/*

ICHICKEN PLATFORMING ENGINE, WRITTEN BY SCOTT BLACKLOCK
ADHERES TO SEMANTIC VERSIONING 2.0.0 (https://semver.org/spec/v2.0.0.html)

CHANGELOG

[ 0.1.1 -- 12/29/2020 ]
Mini-refactor, splitting functions into seperate .CPP files.
Next: Replace `std::map<std::string, std::pair<size_t, char*>> ASSETS`
         with `std::map<std::string, std::string> ASSETS` as it's basically the same
         as a pair.

[ 0.1.0 -- 12/27/2020 ]
Decoding files from ZIP is working!

[ 0.0.0 -- 12/27/2020 ]
Initial creation.
SDL_mixer testing... we now can play MIDIs!

[ PLAN -- 12/27/2020 ]
My current plan for this project is to have an asset pipeline that loads any expected
assets before the level actually starts, and load anything extra on the fly as a fallback.
I'm thinking of something like a tree of assets -- like so

[ ] GAME FILESYSTEM
 |
 +--[ ] INTRO LOGOS / TITLESCREEN
 |   |
 |   +-- logo1.png
 |   +-- logo2.png
 |   +-- titlescreen.png
 |   +-- logoscript.???
 |
 +--[ ] ROOMS, TILESETS, BACKGROUNDS
 |   |
 |   +-- tileset.png
 |   +-- bg1.png
 |   +--[ ] room1.???
 |       |
 |       +-- Assets to load
 |       +-- Tileset
 |       +-- Music track
 |       +-- Background
 |       +-- Map data
 |       +-- Object data (spawnpoint, enemies)
 |       +-- Collsiion data
 |
 +--[ ] SPRITES
     |
     +--[ ] player
     |   |
     |   +-- spritedefs.???
     |   +-- walk1.png
     |   +-- walk2.png
     |   +-- walk3.png
     |       ...
     |
     +--[ ] enemy
         |
         +-- spritedefs.???
         +-- normal.png
         +-- attack1.png
         +-- attack2.png
         +-- dead.png
             ...

Current dependencies:
`vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-mixer[nativemidi] kubazip simpleini`

Since this is basically babby's first C++ game project, it's extremely likely this
will need many, many, refactors.

*/

// Lots of SDL stuff heavily adapted from Lazy Foo Productions
// https://lazyfoo.net/tutorials/SDL/index.php

#include "globals.h"
#include "Util.h"
#include "AssetMan.h"

/////////////////////

Mix_Music* music = NULL;

bool initSDLShit() {
    bool t; // temp
    
    t = SDL_Init(SDL_INIT_AUDIO) < 0;
    ASSERT(t, "Couldn't initialize SDL! SDL Err." << SDL_GetError());

    t = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0;
    ASSERT(t, "Couldn't initialize Mixer! Mix Err." << Mix_GetError());
    
    initAssetManager();

    std::string musicName = fromGameIni("game", "music");
    std::cout << "Loading " << musicName << std::endl;
    loadAsset(musicName);
    music = Mix_LoadMUSType_RW(SDL_RWFromMem(getAsset(musicName), getAssetSize(musicName)), MUS_MID, 0);
    ASSERT(music == NULL, "Couldn't load mus_test!");

    return true;
}

void destruct() {
    destructAssetManager();
    Mix_FreeMusic(music);
    Mix_Quit();
    SDL_Quit();
}

int main()
{
    if (!initSDLShit()) { return 1; }
    std::cout << "iChicken: Now playing music" << std::endl;
    Mix_PlayMusic(music, -1);
    const int tm = 30;
    for (int ti = 0; ti < tm; ti++) {
        std::cout << "\rPlaying for " << tm-ti << " more seconds  ";
        SDL_Delay(1000);
    }
    destruct();
}