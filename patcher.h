//
// Created by RedbeanW on 9/16/2022.
//

#ifndef MCPATCHER_PATCHER_H
#define MCPATCHER_PATCHER_H

#include <vector>
#include <unordered_map>
#include <fstream>
#include <utility>
#include "logger.h"

#include "utils.h"

using std::vector;
using std::unordered_map;
using std::string;
using std::pair;
using std::fstream;
using std::ios;

enum class Platform {
    Win10 = 0x1
};

namespace MCPatcher {

    static unordered_map<Platform,unordered_map<string,vector<pair<vector<unsigned char>,vector<unsigned char>>>>> patches;
    static fstream image;

    void registerPatch(Platform, const string& name, const vector<pair<vector<unsigned char>,vector<unsigned char>>>& patch);
    fstream& getImage();
    bool tryApply(Platform);
    bool open(const string& path);
    void close();
}

#endif //MCPATCHER_PATCHER_H