#pragma once

#include <string>

class User
{
    private:
        char m_Username[32];
        int m_UserID;
        char m_Password[65]; // hashed sha256 sum is 64 char in hex

    public:
        User(std::string & username, std::string & password, int userID);
        ~User();
        void updateUsername(const char *username); // bug will exist here
};