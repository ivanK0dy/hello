#ifndef PARSEXML_H
#define PARSEXML_H
#include <cstring>
#include "rapidxml-1.13/rapidxml.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "functions.h"
#include <list>


void parseXML(char * body, size_t size, std::unordered_map<std::string, std::string> &FTD2);


#endif