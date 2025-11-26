#include "client.hpp"
#include "server.hpp"
#include "log.hpp"
#include <algorithm>
#include <cctype>
#include <string.h>
#include <iostream>


int main(int argc, char* argv[]){

    if(argc < 2){
        std::cout << "Usage: ./program <port_no> [mode (server/client)]" << std::endl;
        return -1;
    }
    if(strlen(argv[1]) != 4){
        std::cout << "port number must be 4 digits" << std::endl;
        return -1;
    }
    if (argc == 2 && argc < 3) {
        std::cout << "Please define mode (server or client) as the second argument." << std::endl;
        return -1;
    }
    /* Convert to lowercase for consistency. */
    std::string mode = argv[2];
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    std::string port_str = "port";
    int port = atoi(argv[1]);

    /* Depending on the mode, start them up differently. */
    if (mode == "server") 
    {
        log_data(port_str, port);
        Server server(port);
        server.run();
    }
    else if (mode == "client")
    {
        std::string ip_addr = "127.0.0.1";
        Client client(ip_addr, port);
        client.run();
    }
    else
    {
        std::cerr << "Invalid argv[2], use 'server' or 'client' (not case-sensitive)..." << std::endl;
        return -1;
    }

    return 0;
}