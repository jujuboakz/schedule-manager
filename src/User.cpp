#include "User.h"
#include <iostream>


User::User()
{
    username = "";
}


bool User::login()
{
    std::cout << "User login" << std::endl;

    // TODO: 后续添加账号密码验证

    return true;
}


bool User::registerUser()
{
    std::cout << "User register" << std::endl;

    // TODO: 后续添加用户注册逻辑

    return true;
}