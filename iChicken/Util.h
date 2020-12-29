#pragma once

#include "globals.h"

std::string concatstrs(int argnum, ...);

// map_has_key() heavily adapted from "Alan (Carson?)'s" SO answer
// https://stackoverflow.com/a/3136537

template<typename K, typename V>
bool map_has_key(std::map<K, V> map, K key) {
    auto it = map.find(key);
    return it != map.end();
}

// ASSERT() adapted from Konrad Rudolph's SO answer
// https://stackoverflow.com/a/3767883
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (condition) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
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

#define CURDIR_dirbufsize 1024
std::string curdir();