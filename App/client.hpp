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

class Client
{
    private:
        int socket_fd; // Client socket file descriptor. 
        std::string server_ip; // IPv4 Address of the server we are connecting to.
        int server_port; // Port NO of the server we are connecting to.
        std::string send_buff; // Place to store messages we send to server.
        std::vector<char> recv_buff; // Place to store messages we recieve from the server.
        bool is_connected; // Connection flag.
        bool was_sent;
        bool quit_flag;

    public:
        Client(const std::string & server_ip, int server_port); // Ctor
        ~Client(); // Should close the socket fd.
        void run(); // Runs all the wrappers below.
        bool get_conn_flag () const { return is_connected; } // Ensures we connect only once.

    private:
        bool connect_to_server(); // Wrapper to connect to TCP server
        void disconnect(); // Disconnect from TCP Server
        bool send_data(); // Send data to suffer in the data buffer. 
        std::string recieve_data(); // Recieve data from the server into the buffer. 
};