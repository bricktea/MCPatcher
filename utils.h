//
// Created by RedbeanW on 9/16/2022.
//

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "Windows.h"
#include <ShObjIdl.h>

using std::string;
using std::vector;

string char2hex(unsigned char chr);
string wchar2string(const wchar_t *wchar);
unsigned long long findBytes(std::fstream& file, const vector<unsigned char> &bytes);
