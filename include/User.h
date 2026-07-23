#ifndef USER_H
#define USER_H

#include <string>

class User
{
public:
    User();

    bool login();

    bool registerUser();

private:
    std::string username;
};

#endif