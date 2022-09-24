//
// Created by RedbeanW on 9/16/2022.
//

#include "utils.h"

string char2hex(unsigned char chr)
{
    /*for (auto i : tmpVec){
    }*/
    std::ostringstream ss;
    auto bin = int(chr);
    ss << std::hex << bin;
    return ss.str();
}

string wchar2string(const wchar_t *wchar)
{
    const wchar_t *wText = wchar;
    DWORD bytes = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,nullptr,0,nullptr,FALSE);
    char *psText;
    psText = new char[bytes];
    WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,psText,bytes,nullptr,FALSE);
    string str = psText;
    delete []psText;
    return str;
}

long long findBytes(std::fstream& file, const vector<unsigned char> &bytes)
{
    unsigned char chr;
    std::vector<unsigned char> tmpVec;
    auto length = bytes.size();
    auto loops = 0;
    file.seekg(0);
    while(file.read((char *)&chr, sizeof(chr)))
    {
        loops++;
        tmpVec.emplace_back(chr);
        auto it = tmpVec.rbegin();
        auto pos = 0;
        while (it != tmpVec.rend())
        {
            pos++;
            if (pos > length || bytes.at(length - pos) != *it)
                break;
            else if (pos == length)
                return loops - length;
            ++it;
        }
    }
    return 0;
}