#pragma once

#include "globals.h"
#include "Util.h"

bool ASSETS_has_key(std::string key);
bool ASSETFILENAMES_has_key(std::string key);
void initAssetManager();
void loadAssetbyFN(std::string filename);
void loadAsset(std::string name);
//void loadAssets(std::list<std::string> assetNames);
void unloadAsset(std::string name);
void checkAssetLoaded(std::string name);
const char* getAsset(std::string name);
int getAssetSize(std::string name);
std::string fromGameIni(std::string section, std::string key);
void destructAssetManager();
std::string getAssetString(std::string name);
SDL_RWops* getAssetRWops(std::string name);