#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"

// https://linuxhint.com/parse_xml_in_cpp/c

using namespace std;
using namespace rapidxml;

xml_document<> doc;
xml_node<> * root_node = NULL;

void parseXML(vector<char> &buffer) {
    cout << "\nParsing recieved data" << endl;
   
    // Read the sample.xml file
    //ifstream theFile ("sample.xml");
    //vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    
    buffer.push_back('\0');
   
    // Parse the buffer
    doc.parse<0>(&buffer[0]);
   
    // Find out the root node
    root_node = doc.first_node("MyStudentsData");
   
    // Iterate over the student nodes
    for (xml_node<> * student_node = root_node->first_node("Student"); student_node; student_node = student_node->next_sibling())
    {
        cout << "\nStudent Type =   " <<
                student_node->first_attribute("student_type")->value();
        cout << endl;
           
            // Interate over the Student Names
        for(xml_node<> * student_name_node = student_node->first_node("Name");
                student_name_node; student_name_node = student_name_node->next_sibling())
        {
            cout << "Student Name =   " << student_name_node->value();
            cout << endl;
        }
        cout << endl;
    }
    buffer.push_back('\0');
    
   // for( char n : buffer)   std::cout << n ;
    
   
    //return 0;
}