#include "parsexml.h"
using namespace std;
using namespace rapidxml;

// https://linuxhint.com/parse_xml_in_cpp/c



xml_document<> doc;
xml_node<> * root_node = NULL;

void parseXML(char * body, size_t size, std::unordered_map<std::string, std::string>  &FTD2) {
    std::cout << "\nParsing recieved data" << std::endl;
    
    char* input = new char[size +1] {0};
    strcpy(input, body);
    
    // Parse the buffer
    
    doc.parse<0>(body);
   
    // Find out the root node
    root_node = doc.first_node("DevEUI_uplink");
   
    // Decode Required Data
    // Get Data from XML + Check LIST for KeyValues and then set pairs!

    //
    std::list<std::string> SpecificParameters = CatchSpecificArguments("AdonisFTDParms");
    std::list<std::string>::iterator it;
    string ARG;
    
    for (xml_node<> *node = root_node->first_node("DevEUI", 0, false);
     node; node = node->next_sibling())
                {
                                      
                //        cout << "Attribute: " << node->name() << " ";
                //        cout << "with value " << node->value() << "\n";
                ARG = node->name();
                

                for (it = SpecificParameters.begin(); it != SpecificParameters.end(); ++it) {
                        if (*it == ARG) {
                                                       
                                FTD2.insert( std::make_pair(node->name(), node->value()) );
                                
                            }
                        /*

                        if (strcmp(node->name(), "payload_hex") == 0) {
                                FTD2.insert( std::make_pair(node->name(), node->value()) );
                        }
                       */
                }
                
        }
                
        

        
     // Free heap memory.
     delete[] input;
   
}