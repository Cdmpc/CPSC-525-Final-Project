#pragma once

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "log.cpp"

class User
{
    char m_Username[32];
    int m_UserID;
    char m_Password[65]; // hashed sha256 sum is 64 char in hex

public:
    User(std::string username, std::string password, int userID);
    ~User();
    void updateUsername(const char *username); // bug will exist here
};

class Server
{
    // member variables
    int m_numUsers;
    int m_Port;
    std::vector<User> m_Users; // because each will be pushed back as they are created this should stay sorted by userID

public:
    Server(int port);
    ~Server();
    void run(); // call bind then listen

private:
    // member functions
    void bind();                          // bind socket to supplied port
    void listen();                        // wait for connections, handle message in loop
    void handleMessage();                 // message payload based on defined payload protocol
    bool verifyPassword();                // compare hashes
    std::string getHash();                // hash passwords for storing and comparing
    void storeNote(std::string contents); //  open file, store contents(will overwrite)
    std::string getNote();                // open file, read contents to string, return string (we could do encryption)
    int getID(std::string username);      // return -1 if doesn't exist, enhached for loop through vector to find match
};
