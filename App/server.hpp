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
#include "log.hpp"

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
    bool m_connection = false;
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
    std::string sha256(std::string& password);
};
