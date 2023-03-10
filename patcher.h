//
// Created by RedbeanW on 9/16/2022.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <fstream>
#include <utility>

#include "logger.h"
#include "utils.h"

using std::vector;
using std::unordered_map;
using std::string;
using std::fstream;

enum class Platform {
    Win10
};

class MCPatcher {
public:

    ~MCPatcher();

    using BinarySequence = vector<unsigned char>;

    struct SinglePatch {
        BinarySequence mBefore;
        BinarySequence mAfter;
    };

    void registerPatch(Platform, const string& name, const vector<SinglePatch>& patch);

    bool target(const string& path);

    bool apply(Platform);

    fstream& getImage();

    unordered_map<string, vector<SinglePatch>>& getPatches(Platform);

private:

    unordered_map<Platform, unordered_map<string, vector<SinglePatch>>> mPatches;
    fstream mImage;

};
