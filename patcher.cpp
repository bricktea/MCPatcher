//
// Created by RedbeanW on 9/16/2022.
//

#include "patcher.h"

using std::cout;
using std::endl;

void MCPatcher::registerPatch(Platform platform, const string& name, const vector<pair<vector<unsigned char>,vector<unsigned char>>>& patch) {
    for (auto& i : patch)
    {
        if (i.first.size() != i.second.size())
        {
            cout << "[x] The wrong patch is being registered!" << endl;
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
        cout << "[i] Trying \"" << it.first << "\" patch." << endl;
        cout << "[i] Need to find " << it.second.size() << " binary position..." << endl;
        needModify.clear();
        auto count = 0;
        for (auto& bin : it.second)
        {
            count++;
            auto pos = findBytes(image,bin.first);
            if (pos)
            {
                cout << "[i] Point " << count << " founded, " << pos << "." << endl;
                needModify.emplace_back(pair{pos,bin.second});
            }
            else
            {
                cout << "[i] Point " << count << " not found, try the next set." << endl;
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
