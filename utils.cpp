//
// Created by RedbeanW on 9/16/2022.
//

#include "utils.h"

#include <sstream>
#include <charconv>

string char2hex(BYTE chr) {
    std::ostringstream ss;
    auto bin = int(chr);
    ss << std::hex << bin;
    return ss.str();
}

unsigned char hex2char(std::string_view str) {
    unsigned int value;
    std::from_chars(str.data(), str.data() + str.size(), value, 16);
    return static_cast<unsigned char>(value);
}

string wchar2string(const wchar_t *wchar) {
    const wchar_t *wText = wchar;
    DWORD bytes = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,nullptr,0,nullptr,FALSE);
    char *psText;
    psText = new char[bytes];
    WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,psText,bytes,nullptr,FALSE);
    string str = psText;
    delete []psText;
    return str;
}
