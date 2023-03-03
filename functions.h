#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <algorithm>
#include <sstream>
// git clone https://github.com/brofield/simpleini.git
#include "simpleini/SimpleIni.h"
#include "mydebug.h"
#include <sys/socket.h>
// git clone https://github.com/brofield/simpleini.git
#include "simpleini/SimpleIni.h"
#include <unordered_map>
#include <map>
#include <list>



std::string trim(const std::string& str, const std::string& white = " \t\r");

std::string reduce( const std::string &str, const std::string &fill = " ", const std::string &whitespace = " \t");

// Print MAP KEY: VALUES
void printMAP (std::map<std::string,std::string> &map);
void printMAP (std::unordered_map<std::string,std::string> &map);

// Process and populate parameters from GET/POST into MAP (destination_MAP, char_*input)
void processPOSTquery (std::unordered_map<std::string, std::string> &dest_map, const char * input_char);

// Send Reply to connection
void ConnSendReply(int &clientfd); 

 std::list<std::string>  CatchSpecificArguments(const char *PARM, const char * CONFIG = "config.ini");
#endif