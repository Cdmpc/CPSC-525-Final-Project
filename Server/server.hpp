/* 

This server is intended to work exclusively with its Client.
The Client/Server protocols can be found in protocols.txt

This server is not persistent. If it crashes all information
is lost, with then exception of any created secret files. User credetials
will be lost.

proper shut down that deletes the Secrets as well and can be done by
using a network utility such as nc

$ nc 127.0.0.1 7000
EXIT master shutdown *

When a new user is created on the sever their credentials,
username, password and file handle will be stored in the 
users vector on the heap.

When they logoff using the client their secrect will be stored
to their file handle.

When they login using the client they will receive the contents
of the file.

When they update their username or password this will be reflected
in their credentails within the users vector.

*/

#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <netdb.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <cerrno>
#include "log.cpp"

class Server
{
    struct User{
        char username[32] = {};
        char handle[32] = {};
        char password[32] = {};
    };

    const size_t MAX_PAYLOAD = 4096 - (32 * 3);
    const char* m_handlePre = "user";
    const char* m_handlePost = ".txt";
    std::string m_created = "CREATED";
    std::string m_stored = "STORED";
    std::string m_invalid = "INVALID";
    std::string m_taken = "TAKEN";
    std::filesystem::path m_secrets = "./Secrets";
    std::string m_path = "./Secrets/";
    int m_serverSocket;
    int m_clientSocket;
    int m_numUsers = 1;
    uint16_t m_Port;
    int m_user_index;
    char m_recv[4096];
    char m_send[4096];
    char m_password[32];
    char m_username[32];
    char m_command[32];
    char m_payload[4096];
    ssize_t m_bytesRecv = 0;
    ssize_t m_bytesSent = 0;
    ssize_t m_payloadSize = 0;
    std::vector<User> m_users; 
    User m_user;
    bool m_running = true;

public:
    
    Server(uint16_t port);
    ~Server();  
    void run(); 

private:

    void bind_wrapper();                          
    void listen_wrapper();                        
    int handle_message(); 
    bool user_exist();                
    bool verify_password();
    bool verify_user();                
    void store_note();         
    void get_note();                
};
