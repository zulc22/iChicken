/*

ICHICKEN ASSET MANAGER

*/

#include "globals.h"
#include "Util.h"
#include "AssetMan.h"
#include "AssetMan_internalvars.h"

namespace AssetPipeline {
	std::map<std::string, std::string> ASSETFILENAMES;
	std::map<std::string, std::string> ASSETS;

	CSimpleIniA game_ini;
	zip_t* game_zip;

	bool ASSETS_has_key(std::string key) {
		return map_has_key<std::string, std::string>(ASSETS, key);
	}
	bool ASSETFILENAMES_has_key(std::string key) {
		return map_has_key<std::string, std::string>(ASSETFILENAMES, key);
	}

	bool readGameIni = false;
	void initAssetManager() {
		if (readGameIni) return;
		readGameIni = true;

		std::cout << "Initializing asset pipeline" << std::endl;

		game_zip = zip_open(GAMEZIP.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
		loadAssetbyFN("game.ini");
		SI_Error rc = game_ini.LoadData(ASSETS["game.ini"].c_str());
		ERRIF(rc != SI_OK, "Unable to load game.ini");

		std::cout << "Discovering assets... ";
		std::list<CSimpleIniA::Entry> assets;
		game_ini.GetAllKeys("assets", assets);
		for (const CSimpleIniA::Entry& ei : assets) {
			std::cout << ei.pItem << " ";
			ASSETFILENAMES[ei.pItem] = game_ini.GetValue("assets", ei.pItem);
		}
		std::cout << std::endl;
		assets.clear();
	}

	void loadAssetbyFN(std::string filename) {
		initAssetManager();
		ASSETFILENAMES[filename] = filename;
		loadAsset(filename);
	}

	void loadAsset(std::string name) {
		initAssetManager();
		bool knownname = ASSETFILENAMES_has_key(name);
		bool loaded = ASSETS_has_key(name);
		ERRIF(!knownname, "Tried to load unknown asset " << name);
		if (loaded) return;

		zip_entry_open(game_zip, ASSETFILENAMES[name].c_str());
		size_t size = zip_entry_size(game_zip) + 1;
		void* asset = NULL;
		zip_entry_read(game_zip, &asset, &size);
		zip_entry_close(game_zip);

		ASSETS[name] = std::string((char*)asset, size);
	}
	
	/*void loadAssets(std::list<std::string> assetNames) {
		for (const std::string ai : assetNames) {
			loadAsset(ai);
		}
	}*/
	
	void unloadAsset(std::string name) {
		ASSETS.erase(name);
	}

	#define logWARNSYM "/!\\ "
	void checkAssetLoaded(std::string name) {
		bool knownname = ASSETFILENAMES_has_key(name);
		bool loaded = ASSETS_has_key(name);
		if (!loaded && knownname) {
			std::cout << logWARNSYM << "Loading asset " << name << " on the fly -- AVOID this when possible!" << std::endl;
			loadAsset(name);
		}
		ERRIF(!knownname, "No such asset " << name << " is known.");
	}

	char* getAsset(std::string name) {
		initAssetManager();
		checkAssetLoaded(name);
		return ASSETS[name].data();
	}

	int getAssetSize(std::string name) {
		initAssetManager();
		checkAssetLoaded(name);
		return ASSETS[name].size();
	}

	std::string fromGameIni(std::string section, std::string key) {
		const char* valueChr = game_ini.GetValue(section.c_str(), key.c_str());
		ERRIF(valueChr == NULL, "Tried to read " << section << "." << key << " from game.ini. Not found...");
		return std::string(valueChr);
	}

	void destructAssetManager() {
		zip_close(game_zip);
	}

	std::string* getAssetString(std::string name) {
		initAssetManager();
		checkAssetLoaded(name);
		return &ASSETS[name];
	}
}