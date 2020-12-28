/*

ICHICKEN PLATFORMING ENGINE, WRITTEN BY SCOTT BLACKLOCK
ADHERES TO SEMANTIC VERSIONING 2.0.0 (https://semver.org/spec/v2.0.0.html)

CHANGELOG

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

#include <SDL2/SDL.h>
#undef main

#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdarg>
#include <SimpleIni.h>
#include <kubazip/zip/zip.h>
#include <map>

//UTILITY FUNCTIONS//

std::string concatstrs(int argnum, ...) {
    va_list  valist;
    va_start(valist, argnum);

    std::string output;

    for (int vli = 0; vli < argnum; vli++) {
        output.append(va_arg(valist, std::string));
    }

    va_end(valist);

    return output;
}

template<typename K, typename V>
bool map_has_key(std::map<K,V> map, K key) {
    auto it = map.find(key);
    return it != map.end();
}

///// ASSERTION /////

// Adapted from Konrad Rudolph's SO answer
// https://stackoverflow.com/a/3767883

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (condition) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

///// CWD STUFF /////

// Adapted from Anvi Jain's article on tutorialspoint
// https://www.tutorialspoint.com/find-out-the-current-working-directory-in-c-cplusplus

#ifdef _WIN32
#include <direct.h>
#define curdirbuf _getcwd
#else
#include <unistd.h>
#define curdirbuf getcwd
#endif

std::string curdir() {
#define dirbufsize 1024

    char buf[dirbufsize];
    char* cdbo = curdirbuf(buf, dirbufsize);
    ASSERT(cdbo == NULL, "Couldn't get current directory");
    return std::string(buf);

#undef dirbufsize
}

// ASSET MANAGMENT //

#define GAMEZIP "testgame.zip"

std::map<std::string, std::string> ASSETFILENAMES;
std::map<std::string, std::pair<size_t, char*>> ASSETS;

CSimpleIniA game_ini;
zip_t* game_zip;

bool ASSETS_has_key(std::string key) {
    return map_has_key<std::string, std::pair<size_t, char*>>(ASSETS, key);
}
bool ASSETFILENAMES_has_key(std::string key) {
    return map_has_key<std::string, std::string>(ASSETFILENAMES, key);
}

bool readGameIni = false;
void checkAssetTable() {
    if (readGameIni) return;
    readGameIni = true;

    game_zip = zip_open(GAMEZIP, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    zip_entry_open(game_zip, "game.ini");
    ASSETS["game.ini"] = { (size_t)zip_entry_size(game_zip), NULL };
    ASSETS["game.ini"].second = NULL;
    zip_entry_read(game_zip, (void**)&ASSETS["game.ini"].second, &ASSETS["game.ini"].first);
    zip_entry_close(game_zip);
    ASSETS["game.ini"].second[ASSETS["game.ini"].first] = 0x00;

    SI_Error rc = game_ini.LoadData(ASSETS["game.ini"].second);
    ASSERT(rc != SI_OK, "Unable to load game.ini");

    std::list<CSimpleIniA::Entry> assets;
    game_ini.GetAllKeys("assets", assets);
    for (const CSimpleIniA::Entry ei : assets) {
        std::cout << ei.pItem << std::endl;
        ASSETFILENAMES[ei.pItem] = game_ini.GetValue("assets", ei.pItem);
    }
    assets.clear();
}

void loadAsset(std::string name) {
    checkAssetTable();
    bool knownname = ASSETFILENAMES_has_key(name);
    bool loaded = ASSETS_has_key(name);
    ASSERT(!knownname, "Tried to load unknown asset " << name);
    if (loaded) return;

    zip_entry_open(game_zip, ASSETFILENAMES[name].c_str());
    ASSETS[name] = { (size_t)zip_entry_size(game_zip), NULL };
    zip_entry_read(game_zip, (void**)&ASSETS[name].second, &ASSETS[name].first);
    zip_entry_close(game_zip);
}
/*
void loadAssets(std::list<std::string> assetNames) {
    for (const std::string ai : assetNames) {
        loadAsset(ai);
    }
}
*/
void unloadAsset(std::string name) {
    free(ASSETS[name].second);
    ASSETS.erase(name);
}

char* getAsset(std::string name) {
    checkAssetTable();
    bool knownname = ASSETFILENAMES_has_key(name);
    bool loaded = ASSETS_has_key(name);
    if (!loaded && knownname) {
        std::cout << "WARNING: Loading asset " << name << " on the fly -- this shouldn't happen!" << std::endl;
        loadAsset(name);
    }
    ASSERT(!knownname, "No such asset " << name << " is known.");
    return ASSETS[name].second;
}

int getAssetSize(std::string name) {
    checkAssetTable();
    bool knownname = ASSETFILENAMES_has_key(name);
    bool loaded = ASSETS_has_key(name);
    if (!loaded && knownname) {
        std::cout << "WARNING: Loading asset " << name << " on the fly -- this shouldn't happen!" << std::endl;
        loadAsset(name);
    }
    ASSERT(!knownname, "No such asset " << name << " is known.");
    return ASSETS[name].first;
}

/////////////////////

Mix_Music* music = NULL;

bool initSDLShit() {
    bool t; // temp
    
    t = SDL_Init(SDL_INIT_AUDIO) < 0;
    ASSERT(t, "Couldn't initialize SDL! SDL Err." << SDL_GetError());

    t = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0;
    ASSERT(t, "Couldn't initialize Mixer! Mix Err." << Mix_GetError());
    
    music = Mix_LoadMUSType_RW(SDL_RWFromMem(getAsset("mus_test"), getAssetSize("mus_test")), MUS_MID, 0);
    ASSERT(music == NULL, "Couldn't load mus_test!");

    return true;
}

void destruct() {
    zip_close(game_zip);
    Mix_FreeMusic(music);
    Mix_Quit();
    SDL_Quit();
}

int main()
{
    if (!initSDLShit()) { return 1; }
    std::cout << "iChicken: SDL_mixer test\n";
    Mix_PlayMusic(music, -1);
    SDL_Delay(30 * 1000);
    destruct();
}