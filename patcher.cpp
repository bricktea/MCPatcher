//
// Created by RedbeanW on 9/16/2022.
//

#include "patcher.h"

void MCPatcher::registerPatch(Platform platform, const string& name, const vector<pair<vector<unsigned char>,vector<unsigned char>>>& patch) {
    for (auto& i : patch)
    {
        if (i.first.size() != i.second.size())
        {
            Error("The wrong patch is being registered!");
            return;
        }
    }
    patches[platform][name] = patch;
}

bool MCPatcher::open(const string &path) {
    image.open(path,ios::binary|ios::in|ios::out);
    return image.is_open();
}

void MCPatcher::close() {
    return image.close();
}

fstream& MCPatcher::getImage() {
    return image;
}

bool MCPatcher::tryApply(Platform platform) {
    auto tryuse = patches[platform];
    vector<std::pair<long long,vector<unsigned char>>> needModify;
    auto isOk = true;
    for (auto& it : tryuse)
    {
        Info("Trying \"{}\" patch.",it.first);
        Info("Need to find {} binary position...",it.second.size());
        needModify.clear();
        auto count = 0;
        for (auto& bin : it.second)
        {
            count++;
            auto pos = findBytes(image,bin.first);
            if (pos)
            {
                Info("Point {} founded, {}.",count,pos);
                needModify.emplace_back(pair{pos,bin.second});
            }
            else
            {
                Warn("Point {} not found, try the next set.",count);
                isOk = false;
                break;
            }
        }
        if (isOk)
            break;
    }
    if (!isOk || needModify.empty())
        return false;
    for (auto& patch : needModify)
    {
        image.seekg(patch.first);
        for (auto& bts : patch.second)
        {
            image.write((char *)&bts, sizeof(bts));
        }
    }
    return image.good();
}
