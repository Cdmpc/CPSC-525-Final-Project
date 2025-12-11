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
#include <cstddef>
#include <cerrno>
#include "log.cpp"
#include <algorithm>

class Client
{
    std::string m_username;
    std::string m_new_username;
    std::string m_password;
    std::string m_new_password;

    const size_t MAX_PAYLOAD = 4096 - (32 * 3);
    std::string m_command = "";
    std::string m_response = "";
    std::string m_created = "CREATED";
    std::string m_stored = "STORED";
    std::string m_invalid = "INVALID";
    std::string m_taken = "TAKEN";
    std::filesystem::path m_secret = "./Secret";
    std::string m_path = "./Secret/";
    int m_clientSocket;
    uint16_t m_port;
    std::string m_hostname;
    char m_recv[4096];
    char m_send[4096];
    char m_payload[4096];
    ssize_t m_bytesRecv = 0;
    ssize_t m_bytesSent = 0;
    bool m_editing = true;
    bool m_connected = false;

public:

    Client(std::string hostname, uint16_t port);
    ~Client();
    void run();

private:
    
    void returning_user();
    void get_username();
    void get_new_username();
    void get_password();
    void get_new_password();
    void connect_to_server();
    void create_user();
    void get_note();
    void handle_commands();
    void execute_command();
    void store_note();
    void update_username();
    void update_password();
    void create_file_to_edit();
    void file_to_payload();

};
