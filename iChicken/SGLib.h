#pragma once
#include "globals.h"
#include "AssetMan.h"

namespace Multimedia {
	bool init();
	void playMusic(std::string name);
	void unload();
}