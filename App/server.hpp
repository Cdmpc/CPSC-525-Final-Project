#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <netdb.h>
#include "user.hpp"

class Server
{
    private:
        // member variables, m_ denotes them.
        int m_serverSocket;
        int m_numUsers;
        uint16_t m_Port;
        std::vector<User> m_Users; // because each will be pushed back as they are created this should stay sorted by userID

    public:
        Server(uint16_t port); // ctor
        ~Server();  // dtor
        void run(); // call bind then listen

    private:
        // member functions
        void bind_wrapper();                          // bind socket to supplied port
        void listen_wrapper();                        // wait for connections, handle message in loop
        void handle_message();                 // message payload based on defined payload protocol
        bool verify_password();                // compare hashes
        std::string & get_hash();                // hash passwords for storing and comparing
        void store_note(std::string & contents); //  open file, store contents(will overwrite)
        std::string & get_note();                // open file, read contents to string, return string (we could do encryption)
        int get_id(std::string & username);      // return -1 if doesn't exist, enhached for loop through vector to find match
};
