#include "server.hpp"
#include "log.hpp"
#include <string.h>
#include <iostream>


int main(int argc, char* argv[]){

    if(argc < 2){
        std::cout << "please define port number" << std::endl;
        return -1;
    }
    if(strlen(argv[1]) != 4){
        std::cout << "port number must be 4 digits" << std::endl;
        return -1;
    }
    std::string port_str = "port";
    int port = atoi(argv[1]);

    log(port_str, port);

    // create server and run, run loops
    Server server(port);
    server.run();

    return 0;
}