//
// Created by RedbeanW on 9/16/2022.
//

#include "patcher.h"

void MCPatcher::registerPatch(Platform platform, const string& name, const vector<SinglePatch>& patch) {
    for (auto& i : patch)
    {
        if (i.mBefore.size() != i.mAfter.size())
        {
            Error("The wrong patch is being registered!");
            return;
        }
    }
    mPatches[platform][name] = patch;
}

bool MCPatcher::apply(Platform platform) {
    vector<std::pair<long long, BinarySequence>> needModify;
    auto isOk = true;
    for (auto& it : mPatches[platform])
    {
        Info("Trying \"{}\" patch.", it.first);
        Info("Need to find {} binary position...", it.second.size());
        needModify.clear();
        auto count = 0;
        for (auto& bin : it.second)
        {
            count++;
            auto pos = findBytes(mImage, bin.mBefore);
            if (pos)
            {
                Info("Point {} founded, {}.", count, pos);
                needModify.emplace_back(std::pair{pos, bin.mAfter});
                isOk = true;
            }
            else
            {
                Warn("Point {} not found, try the next set.", count);
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
        mImage.seekg(patch.first);
        for (auto& bts : patch.second)
        {
            mImage.write((char *)&bts, sizeof(bts));
        }
    }
    return mImage.good();
}

bool MCPatcher::target(const string& path) {
    mImage.open(path, std::ios::binary | std::ios::in | std::ios::out);
    return mImage.is_open();
}

fstream& MCPatcher::getImage() {
    return mImage;
}

unordered_map<string, vector<MCPatcher::SinglePatch>>& MCPatcher::getPatches(Platform platform) {
    return mPatches[platform];
}

MCPatcher::~MCPatcher() {

    mImage.close();

}
