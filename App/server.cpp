#include "server.hpp"



void Server::bind()
{
    // socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // IPv4 adresss
    serverAddress.sin_port = htons(m_Port); // supplied port
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept all connections

}


