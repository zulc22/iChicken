/*

ICHICKEN ASSET MANAGER

*/

#include "globals.h"
#include "AssetMan.h"
#include "Util.h"

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
void initAssetManager() {
    if (readGameIni) return;
    readGameIni = true;

    std::cout << "Initializing asset pipeline" << std::endl;

    game_zip = zip_open(GAMEZIP, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    zip_entry_open(game_zip, "game.ini");
    ASSETS["game.ini"] = { (size_t)zip_entry_size(game_zip), NULL };
    ASSETS["game.ini"].second = NULL;
    zip_entry_read(game_zip, (void**)&ASSETS["game.ini"].second, &ASSETS["game.ini"].first);
    zip_entry_close(game_zip);
    ASSETS["game.ini"].second[ASSETS["game.ini"].first] = 0x00;

    SI_Error rc = game_ini.LoadData(ASSETS["game.ini"].second);
    ASSERT(rc != SI_OK, "Unable to load game.ini");

    std::cout << "Assets discovered from game.ini:" << std::endl;
    std::list<CSimpleIniA::Entry> assets;
    game_ini.GetAllKeys("assets", assets);
    for (const CSimpleIniA::Entry ei : assets) {
        std::cout << ei.pItem << std::endl;
        ASSETFILENAMES[ei.pItem] = game_ini.GetValue("assets", ei.pItem);
    }
    std::cout << std::endl;
    assets.clear();
}

void loadAsset(std::string name) {
    initAssetManager();
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
    initAssetManager();
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
    initAssetManager();
    bool knownname = ASSETFILENAMES_has_key(name);
    bool loaded = ASSETS_has_key(name);
    if (!loaded && knownname) {
        std::cout << "WARNING: Loading asset " << name << " on the fly -- this shouldn't happen!" << std::endl;
        loadAsset(name);
    }
    ASSERT(!knownname, "No such asset " << name << " is known.");
    return ASSETS[name].first;
}

std::string fromGameIni(std::string section, std::string key) {
    const char* valueChr = game_ini.GetValue(section.c_str(), key.c_str());
    ASSERT(valueChr == NULL, "Tried to read " << section << "." << key << " from game.ini. Not found...");
    return std::string(valueChr);
}

void destructAssetManager() {
    zip_close(game_zip);
}