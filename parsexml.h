#ifndef PARSEXML_H
#define PARSEXML_H
#include "rapidxml-1.13/rapidxml.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace rapidxml;

void parseXML(char * body, size_t size, std::unordered_map<std::string, std::string>  &FTD2);


#endif