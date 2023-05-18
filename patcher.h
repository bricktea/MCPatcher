//
// Created by RedbeanW on 9/16/2022.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <fstream>
#include <utility>

#include "utils.h"

using std::vector;
using std::unordered_map;
using std::string;
using std::fstream;

enum class Platform {
    Win10,
    Android,
    IOS
};

class MCPatcher {
public:

    ~MCPatcher();

    enum class DataType {
        NORMAL,     // e.g. 0E
        ALL        // e.g. ??
    };

    struct ByteEntity {
        DataType mType     = DataType::NORMAL;
        BYTE mData         = 0x00;
        struct Replacer {
            bool mEnabled = false;
            BYTE mData    = 0x00;
        } mReplacer;
    };

    struct PatchEntity {
        vector<ByteEntity> mBytes;
        void add(const ByteEntity& entity) {
            mBytes.emplace_back(entity);
        };
        [[nodiscard]] bool valid() const {
            return !mBytes.empty();
        };
    };

    void registerPatch(Platform platform, const string& name, const PatchEntity& patch);

    bool target(const string& path);

    bool apply(Platform);

    fstream& getImage();

    unordered_map<string, PatchEntity>& getPatches(Platform);

    static PatchEntity compile(string patchExpression);

private:

    unordered_map<Platform, unordered_map<string, PatchEntity>> mPatches;
    fstream mImage;

    using Address = long long;

    vector<std::pair<Address, BYTE>> handleBytes(const vector<ByteEntity> &bytes);
};

#include <exception>

class CompileFailed : public std::exception {
public:

    [[nodiscard]] const char* what() const noexcept override {
        return "Failed to compile patch expression.";
    };

};
