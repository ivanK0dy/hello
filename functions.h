#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <algorithm>
#include <sstream>


std::string trim(const std::string& str, 
                 const std::string& white = " \t\r")
{
  const auto strBegin = str.find_first_not_of(white);
  if (strBegin == std::string::npos ) return ""; //no content

  const auto strEnd = str.find_last_not_of(white);
  const auto strRange = strEnd - strBegin + 1;
  return str.substr(strBegin, strRange);
}

std::string reduce( const std::string &str, 
                    const std::string &fill = " ", 
                    const std::string &whitespace = " \t")
{
  //trim source first
  auto result = trim(str);

  //replace whitespace in between
  auto BeginSpace = str.find_first_of(whitespace);
  while(BeginSpace != std::string::npos)
  {
    // Find position of first char whic is not whitespace anymore
    const auto endSpace = result.find_first_not_of(whitespace,BeginSpace);
    const auto range = endSpace - BeginSpace;

    //replace xyz char with " " in result
    result.replace(BeginSpace,range, fill);
    const auto newStart = BeginSpace + fill.length();
    // set +1 as we added space " "
    BeginSpace = result.find_first_of(whitespace, newStart);
  }
  return result;
}

// Print MAP KEY: VALUES
void printMAP (auto &map)
{
  cout << "\n------------------------\n";
  for(auto& it: map) {
    std::cout << "KEY: `" << it.first << "`, VALUE: `" << it.second << '`' << std::endl;
  }
  cout << "\n------------------------\n";
}

// Process and populate parameters from GET/POST into MAP (destination_MAP, char_*input)
void processPOSTquery (auto &dest_map, const char * input_char)
{
  std::string s1(input_char);
  size_t strBegin, srchEnd, count {0};
  std::string key, value;
  
  strBegin = s1.find('?',0);
  srchEnd = s1.find(' ',strBegin) - strBegin;

  while(strBegin < srchEnd)
  {
    size_t pos = s1.find_first_of('=',strBegin);
      key = s1.substr(strBegin + 1, pos - strBegin - 1);
    strBegin = s1.find_first_of('&',pos +1);
    
    if (strBegin != std::string::npos) 
      value = s1.substr(pos + 1, strBegin - pos - 1);
    else 
      value = s1.substr(pos + 1, srchEnd - pos - 1);  
    ++count;
    dest_map.insert(std::make_pair(key,value));
  }
}

// Send Reply to connection
void ConnSendReply(int &clientfd) 
{
  std::stringstream reply;
          reply << "HTTP/1.1 200 OK\r\n"
              << "Connection: close\r\n"
              << "Content-Type: application/html;charset=UTF-8\r\n"
              << "Content-Length: 0""\r\n"
              << "Server: AP by Kodi\r\n"     
              << "\r\n";
          const std::string tmp = reply.str();
          const char* creply = tmp.c_str();          
          send(clientfd, creply, tmp.size(), 0);
}


#endif