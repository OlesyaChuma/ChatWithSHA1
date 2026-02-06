// ChatWithSHA1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Chat.h"

int main() {
    Chat chat;

    chat.reg((char*)"user", (char*)"1234", 4);

    std::cout << "Login with correct password: "
        << chat.login((char*)"user", (char*)"1234", 4) << std::endl;

    std::cout << "Login with wrong password: "
        << chat.login((char*)"user", (char*)"1111", 4) << std::endl;
}