#ifndef MYDEBUG_H
#define MYDEBUG_H
#include <string>
#include <iostream>
#include "common.h"
#include <string_view>


void debugOUT(auto a);
void debugOUT(std::string_view a, int b);
void debugOUT(std::string_view a, std::string b);
void debugOUT(std::string_view a, auto ar1, auto ar2);
void debugOUT(std::string_view a, std::string ar1, std::string ar2);
void debugOUT(std::string_view a, std::string_view &b, std::string_view c);

int guard(int n, const char * err);


#endif

