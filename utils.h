//
// Created by RedbeanW on 9/16/2022.
//

#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "Windows.h"

using std::string;
using std::vector;

string char2hex(BYTE chr);
unsigned char hex2char(std::string_view str);
string wchar2string(const wchar_t *wchar);
