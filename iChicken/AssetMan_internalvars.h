#pragma once
#include "globals.h"
#include "AssetMan.h"

namespace AssetPipeline {
	extern std::map<std::string, std::string> ASSETFILENAMES;
	extern std::map<std::string, std::string> ASSETS;

	extern CSimpleIniA game_ini;
	extern zip_t* game_zip;

	const std::string GAMEZIP = "testgame.zip";
}