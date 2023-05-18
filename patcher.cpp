//
// Created by RedbeanW on 9/16/2022.
//

#include "patcher.h"
#include <spdlog/spdlog.h>

void MCPatcher::registerPatch(Platform platform, const string& name, const PatchEntity& patch) {
    if (patch.valid() && !name.empty()) {
        mPatches[platform][name] = patch;
    }
}

bool MCPatcher::apply(Platform platform) {
    vector<std::pair<Address, BYTE>> needModify;
    for (auto& it : mPatches[platform]) {
        spdlog::info("Trying \"{}\" patch.", it.first);
        needModify = handleBytes(it.second.mBytes);
        if (needModify.empty()) {
            spdlog::warn("Byte sequence not found.");
        } else {
            spdlog::info("Successfully found the byte sequence.");
            break;
        }
    }
    if (needModify.empty()) return false;
    for (auto& patch : needModify) {
        mImage.seekg(patch.first - 1);
        mImage.write((char *)&patch.second, sizeof(patch.second));
    }
    return mImage.good();
}

vector<std::pair<MCPatcher::Address, BYTE>> MCPatcher::handleBytes(const vector<ByteEntity> &bytes) {
    if (bytes.empty()) return {};
    vector<std::pair<MCPatcher::Address, BYTE>> rtn;
    mImage.seekg(0);
    int matchedSize = 0;
    BYTE chr;
    while(mImage.read((char *)&chr, sizeof(chr))) {
        auto& byte = bytes.at(matchedSize);
        if (byte.mType == DataType::ALL) {
            matchedSize++;
        } else if (byte.mType == DataType::NORMAL) {
            if (byte.mData == chr) {
                matchedSize++;
            } else {
                matchedSize = 0;
                rtn.clear();
                continue;
            }
        }
        if (byte.mReplacer.mEnabled) {
            rtn.emplace_back(std::pair {mImage.tellg(), byte.mReplacer.mData});
        }
        if (matchedSize == bytes.size()) {
            //for (auto& i : rtn) {
            //    mImage.seekg(i.first);
            //    for (int k = 0; k < 20; k++) {
            //        mImage.read((char *)&chr, sizeof(chr));
            //        spdlog::info("byte -> {}", char2hex(chr));
            //    }
            //    spdlog::info("address -> {}", i.first);
            //}
            return rtn;
        }
    }
    return {};
}

bool MCPatcher::target(const string& path) {
    mImage.open(path, std::ios::binary | std::ios::in | std::ios::out);
    return mImage.is_open();
}

fstream& MCPatcher::getImage() {
    return mImage;
}

unordered_map<string, MCPatcher::PatchEntity>& MCPatcher::getPatches(Platform platform) {
    return mPatches[platform];
}

MCPatcher::~MCPatcher() {
    mImage.close();
}

// e.g. "74 ?? B0 01(00) 48"
// Checkless! Boom if input not correctly!
MCPatcher::PatchEntity MCPatcher::compile(string patchExp) {
    PatchEntity rtn;
    patchExp += " ";
    int nextPosition = 0;
    for (int i = 0; i < patchExp.size(); i++) {
        if (i != nextPosition) continue;
        auto pos = patchExp.find(" ", i, 1);
        if (pos == patchExp.npos) throw CompileFailed();
        auto substr = patchExp.substr(i, pos - i);
        ByteEntity entity;
        if (substr.substr(0, 2) == "??") {
            entity.mType = DataType::ALL;
        } else {
            entity.mType = DataType::NORMAL;
            entity.mData = hex2char(substr.substr(0, 2));
        }
        if (substr.find("(") != substr.npos) {
            entity.mReplacer.mEnabled = true;
            entity.mReplacer.mData = hex2char(substr.substr(3, 2));
        }
        nextPosition = pos + 1;
        rtn.add(entity);
    }
    return rtn;
}