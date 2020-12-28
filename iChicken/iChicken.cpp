/*

ICHICKEN PLATFORMING ENGINE, WRITTEN BY SCOTT BLACKLOCK

CHANGELOG

0.0.0 -- Initial Creation [12/27/2020]
SDL_mixer testing: we now can play MIDIs!

assumed deps are:
vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-mixer[nativemidi] zziplib

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

//// ASSERTION ////

// Adapted from Konrad Rudolph's SO answer
// https://stackoverflow.com/a/3767883

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

//// CWD STUFF ////

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
    ASSERT(curdirbuf(buf, dirbufsize) != NULL, "Couldn't get current directory");
    return std::string(buf);

#undef dirbufsize
}

/////////////////// 

Mix_Music* music = NULL;

bool initSDLShit() {
    bool t; // temp
    
    t = SDL_Init(SDL_INIT_AUDIO) < 0;
    ASSERT(!t, "Couldn't initialize SDL! SDL Err." << SDL_GetError());

    t = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0;
    ASSERT(!t, "Couldn't initialize Mixer! Mix Err." << Mix_GetError());
    
    music = Mix_LoadMUS("test.mid");
    ASSERT(music != NULL, "Couldn't load test.mid! CWD is " << curdir());
    return true;
}

void destruct() {
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