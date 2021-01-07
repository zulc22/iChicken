#pragma once

#include "globals.h"
#include "Util.h"

namespace AssetPipeline {
	bool assetLoaded(std::string key);
	bool assetKnown(std::string key);
	void initAssetManager();
	void loadAssetbyFN(std::string filename);
	void loadAsset(std::string name);
	//void loadAssets(std::list<std::string> assetNames);
	void unloadAsset(std::string name);
	void checkAssetLoaded(std::string name);
	char* getAsset(std::string name);
	int getAssetSize(std::string name);
	std::string fromGameIni(std::string section, std::string key);
	void destruct();
	std::string* getAssetString(std::string name);
}