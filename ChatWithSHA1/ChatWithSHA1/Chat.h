#pragma once

#include "HashTable.h"
#include "sha1.h"

class Chat {
public:
    Chat();
    void reg(char login[LOGINLENGTH], char pass[], int len);
    bool login(char login[LOGINLENGTH], char pass[], int len);

private:
    HashTable users;
};
