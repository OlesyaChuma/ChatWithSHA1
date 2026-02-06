#include "sha1.h"
#include <string.h>

// ----------------------
// Циклический сдвиг влево на bit_count бит
// Используется в алгоритме SHA-1
// ----------------------
uint cycle_shift_left(uint val, int bit_count) {
    return (val << bit_count) | (val >> (32 - bit_count));
}

// ----------------------
// Приведение порядка байт (Big Endian)
// Требуется стандартом SHA-1
// ----------------------
uint bring_to_human_view(uint val) {
    return ((val & 0x000000FF) << 24) |
        ((val & 0x0000FF00) << 8) |
        ((val & 0x00FF0000) >> 8) |
        ((val & 0xFF000000) >> 24);
}

// ----------------------
// Основная функция SHA-1
// message      — сообщение (пароль)
// msize_bytes  — длина сообщения в байтах
//
// Возвращает:
// указатель на массив из 5 uint (20 байт)
// Память должна быть освобождена delete[]
// ----------------------
uint* sha1(char* message, uint msize_bytes) {

    // Инициализация начальных значений хеша
    uint A = H[0];
    uint B = H[1];
    uint C = H[2];
    uint D = H[3];
    uint E = H[4];

    // Подсчёт количества полных 512-битных блоков
    uint totalBlockCount = msize_bytes / one_block_size_bytes;

    // Подсчёт количества байт для дополнения сообщения
    uint needAdditionalBytes =
        one_block_size_bytes - (msize_bytes - totalBlockCount * one_block_size_bytes);

    // Если не хватает места для длины сообщения — добавляем ещё блок
    if (needAdditionalBytes < 8) {
        totalBlockCount += 2;
        needAdditionalBytes += one_block_size_bytes;
    }
    else {
        totalBlockCount += 1;
    }

    // Общий размер дополненного сообщения
    uint extendedMessageSize = msize_bytes + needAdditionalBytes;

    // Выделяем память под новое сообщение
    unsigned char* newMessage = new unsigned char[extendedMessageSize];

    // Копируем исходное сообщение
    memcpy(newMessage, message, msize_bytes);

    // Добавляем бит '1' (0x80), затем нули
    newMessage[msize_bytes] = 0x80;
    memset(newMessage + msize_bytes + 1, 0, needAdditionalBytes - 1);

    // Записываем длину исходного сообщения в битах
    uint* ptr_to_size = (uint*)(newMessage + extendedMessageSize - 4);
    *ptr_to_size = bring_to_human_view(msize_bytes * 8);

    // Расширенный блок (80 слов)
    ExpendBlock exp_block;

    // ----------------------
    // Обработка каждого блока
    // ----------------------
    for (uint i = 0; i < totalBlockCount; i++) {

        // Указатель на текущий блок
        unsigned char* cur_p = newMessage + one_block_size_bytes * i;
        Block block = (Block)cur_p;

        // Первые 16 слов блока
        for (int j = 0; j < one_block_size_uints; j++)
            exp_block[j] = bring_to_human_view(block[j]);

        // Расширение до 80 слов
        for (int j = one_block_size_uints; j < block_expend_size_uints; j++) {
            exp_block[j] =
                exp_block[j - 3] ^
                exp_block[j - 8] ^
                exp_block[j - 14] ^
                exp_block[j - 16];
            exp_block[j] = cycle_shift_left(exp_block[j], 1);
        }

        // Инициализация рабочих переменных
        uint a = A;
        uint b = B;
        uint c = C;
        uint d = D;
        uint e = E;

        // ----------------------
        // Основные 80 раундов SHA-1
        // ----------------------
        for (int j = 0; j < block_expend_size_uints; j++) {
            uint f, k;

            // Выбор функции и константы по номеру раунда
            if (j < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (j < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (j < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            // Основное преобразование
            uint temp = cycle_shift_left(a, 5) + f + e + k + exp_block[j];
            e = d;
            d = c;
            c = cycle_shift_left(b, 30);
            b = a;
            a = temp;
        }

        // Добавляем результаты текущего блока
        A += a;
        B += b;
        C += c;
        D += d;
        E += e;
    }

    // Формируем итоговый хеш (160 бит)
    uint* digest = new uint[SHA1HASHLENGTHUINTS];
    digest[0] = A;
    digest[1] = B;
    digest[2] = C;
    digest[3] = D;
    digest[4] = E;

    // Освобождаем временную память
    delete[] newMessage;

    // Возвращаем хеш
    return digest;
}
