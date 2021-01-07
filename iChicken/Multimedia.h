#pragma once
#include "globals.h"
#include "AssetPipeline.h"

namespace Multimedia {
	bool init();
	void playMusic(std::string name);
	void unload();
}