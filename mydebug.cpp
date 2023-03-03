#include "mydebug.h"
#include "common.h"


using namespace std;

void debugOUT (auto a) {
    if(DEBUG) {
    cout << left;
    cout << "D: " << a << endl;
    }
}

void debugOUT(std::string_view a, int b){
    if(DEBUG) {
    cout << left;
    cout << "D: " << a <<": " << b << endl;
    }
} 
void debugOUT(std::string_view a, string b){
    if(DEBUG) {
    cout << left;
    cout << "D: " << a << ": " << b << endl;
    }
} 
    
void debugOUT(std::string_view a, auto ar1, auto ar2) {
    if(DEBUG) {
    cout << left;
    cout << "D: " << a << " : " << ar1 << " / " << ar2 << endl;
    }
}
void debugOUT(std::string_view a, string ar1, string ar2) {
    if(DEBUG) {
    cout << left;
    cout << "D: " << a << " : " << ar1 << " / " << ar2 << endl;
    }
}


void debugOUT(std::string_view a, std::string_view &b, std::string_view c) {
    if(DEBUG) {
    cout << left;
    cout << "D: " << a << " - " << "AR1: " << &b << " - " << " AR2: " << c << endl;
    }
}

int guard(int n, const char * err) 
{ 
    if (n < 0) 
    {
        debugOUT(err); 
        exit(EXIT_FAILURE);
    }
    return n;
}