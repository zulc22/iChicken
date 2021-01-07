#pragma once

#include "globals.h"

std::string concatstrs(int argnum, ...);

// map_has_key() heavily adapted from "Alan (Carson?)'s" SO answer
// https://stackoverflow.com/a/3136537
namespace Util {
    template<typename K, typename V>
    bool map_has_key(std::map<K, V> map, K key) {
	    auto it = map.find(key);
	    return it != map.end();
    }
}

// ERRIF() heavily adapted from Konrad Rudolph's SO answer
// https://stackoverflow.com/a/3767883

#ifndef NDEBUG
#   define ERRIF(condition, message) \
    do { \
        if (condition) { \
            std::cerr << "ERROR: (" #condition ") == TRUE!" << std::endl \
                      << "(DEBUG BUILD) Caught at: " << __FILE__  << ":L" << __LINE__ << std::endl \
                      << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ERRIF(condition, message) \
    do { \
        if (condition) { \
            std::cerr << "ERROR: (" #condition ") == TRUE!" << std::endl \
                      << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#endif

// curdir() adapted from Anvi Jain's article on tutorialspoint
// https://www.tutorialspoint.com/find-out-the-current-working-directory-in-c-cplusplus

#ifdef _WIN32
#include <direct.h>
#define curdirbuf _getcwd
#else
#include <unistd.h>
#define curdirbuf getcwd
#endif

namespace Util {
    std::string concatstrs(int argnum, ...);

    const int CURDIR_dirbufsize = 1024;
    std::string curdir();
}
