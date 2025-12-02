// to simplify debuging, use to verify a value is correct
#include "log.hpp"

void log(std::string name, int value){
    std::cout << "LOG::" << name << " : " << value << std::endl;
}
void log(std::string name, std::string value){
    std::cout << "LOG::" << name << " : " << value << std::endl;
}
void log(std::string name, char* value){
    std::cout << "LOG::" << name << " : " << value << std::endl;
}
void log(std::string value){
     std::cout << "LOG::" << value << std::endl;
}
