#include "globals.h"
#include "AssetMan.h"
#include "AssetMan_internalvars.h"
#include "SGLib.h"
#include "SGLib_internalvars.h"

namespace AssetPipeline {
	SDL_RWops* getAssetRWops(std::string name) {
		checkAssetLoaded(name);
		return SDL_RWFromMem((void*)ASSETS[name].data(), ASSETS[name].size());
	}
}

namespace Multimedia {
	Mix_Music* music;

	bool init() {
		bool t; // temp
		t = SDL_Init(SDL_INIT_AUDIO) < 0;
		ERRIF(t, "Couldn't initialize SDL! SDL Err." << SDL_GetError());
		t = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0;
		ERRIF(t, "Couldn't initialize Mixer! Mix Err." << Mix_GetError());
		AssetPipeline::initAssetManager();
	}

	void playMusic(std::string name) {
		AssetPipeline::initAssetManager();
		AssetPipeline::checkAssetLoaded(name);
		music = Mix_LoadMUSType_RW(AssetPipeline::getAssetRWops(name), MUS_MID, 0);
		ERRIF(music == NULL, "Unable to load music '" << name << "'!");
		Mix_PlayMusic(music, 0);
	}

	void unload() {
		Mix_FreeMusic(music);
		Mix_Quit();
		SDL_Quit();
	}
}