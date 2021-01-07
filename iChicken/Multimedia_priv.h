#pragma once

#include "globals.h"
#include "Multimedia.h"

namespace Multimedia {
	extern Mix_Music* music;
}

namespace AssetPipeline {
	SDL_RWops* getAssetRWops(std::string name);
}