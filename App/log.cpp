// to simplify debuging, use to verify a value is correct
#include "log.hpp"
#include <iostream>

void log_data(std::string & name, int value){
    std::cout << "LOG::" << name << " : " << value << std::endl;
}
void log_data(std::string & name, std::string & value){
    std::cout << "LOG::" << name << " : " << value << std::endl;
}
void log_data(std::string & value){
     std::cout << "LOG::" << value << std::endl;
}
