#pragma once

#include <string.h>
#include "sha1.h"

#define LOGINLENGTH 10

class HashTable {
public:
    HashTable();
    ~HashTable();

    void add(char login[LOGINLENGTH], uint* hash);
    uint* find(char login[LOGINLENGTH]);
    void del(char login[LOGINLENGTH]);

private:
    enum Status { FREE, ENGAGED, DELETED };

    struct Pair {
        char login[LOGINLENGTH];
        uint* pass_hash;
        Status status;

        Pair() : pass_hash(nullptr), status(FREE) {
            login[0] = '\0';
        }
    };

    Pair* table;
    int size;
    int count;
    int deleted_count;

    int hash_mul(char login[LOGINLENGTH]);
    void resize();
    void rebuild_if_needed();
};
