#include "server.hpp"
#include "log.cpp"



int main(int argc, char* argv[]){

    if(argc < 2){
        std::cout << "please define port number" << std::endl;
    }
    if(std::strlen(argv[1]) != 4){
        std::cout << "port number must be 4 digits" << std::endl;
    }

    int port = atoi(argv[1]);

    log("port", port);

    // create server and run, run loops
    Server server(port);
    server.run();

    return 0;
}