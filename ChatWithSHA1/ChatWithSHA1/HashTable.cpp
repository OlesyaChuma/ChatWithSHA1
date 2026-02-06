#include "HashTable.h"
#include <cmath>

// Константа для метода умножения (золотое сечение)
static const double A = 0.6180339887;

// ----------------------
// Конструктор хеш-таблицы
// ----------------------
HashTable::HashTable() {
    size = 8;              // начальный размер таблицы
    count = 0;             // количество занятых ячеек
    deleted_count = 0;     // количество удалённых ячеек
    table = new Pair[size];
}

// ----------------------
// Деструктор
// Освобождает всю выделенную память
// ----------------------
HashTable::~HashTable() {
    for (int i = 0; i < size; i++)
        if (table[i].status == ENGAGED)
            delete[] table[i].pass_hash; // освобождаем SHA-1 хеш
    delete[] table;
}

// ----------------------
// Хеш-функция методом умножения
// Преобразует логин в индекс таблицы
// ----------------------
int HashTable::hash_mul(char login[LOGINLENGTH]) {
    int sum = 0;
    // Складываем коды символов логина
    for (int i = 0; i < LOGINLENGTH; i++)
        sum += login[i];

    // Берём дробную часть произведения
    double frac = fmod(sum * A, 1.0);
    // Преобразуем в индекс таблицы
    return int(size * frac);
}

// ----------------------
// Добавление элемента
// Используется квадратичное пробирование
// ----------------------
void HashTable::add(char login[LOGINLENGTH], uint* hash) {
    // Проверяем, не требуется ли пересборка таблицы
    rebuild_if_needed();

    int h = hash_mul(login); // базовый индекс

    // Квадратичное пробирование
    for (int i = 0; i < size; i++) {
        int idx = (h + i * i) % size;

        // Вставлять можно в свободную или удалённую ячейку
        if (table[idx].status == FREE || table[idx].status == DELETED) {

            // Если ячейка была удалённой — уменьшаем счётчик deleted
            if (table[idx].status == DELETED)
                deleted_count--;

            // Копируем логин в таблицу
            strcpy_s(table[idx].login, LOGINLENGTH, login);

            // Сохраняем указатель на SHA-1 хеш пароля
            table[idx].pass_hash = hash;

            // Помечаем ячейку как занятую
            table[idx].status = ENGAGED;

            count++; // увеличиваем количество элементов
            return;
        }
    }

    // Если места не нашлось — увеличиваем таблицу и пробуем снова
    resize();
    add(login, hash);
}

// ----------------------
// Поиск элемента по логину
// Возвращает указатель на хеш или nullptr
// ----------------------
uint* HashTable::find(char login[LOGINLENGTH]) {
    int h = hash_mul(login);

    // Квадратичное пробирование
    for (int i = 0; i < size; i++) {
        int idx = (h + i * i) % size;

        // Если дошли до свободной ячейки — элемента нет
        if (table[idx].status == FREE)
            return nullptr;

        // Если ячейка занята и логины совпадают — найдено
        if (table[idx].status == ENGAGED &&
            !strcmp(table[idx].login, login))
            return table[idx].pass_hash;
    }
    return nullptr;
}

// ----------------------
// Удаление элемента по логину
// ----------------------
void HashTable::del(char login[LOGINLENGTH]) {
    int h = hash_mul(login);

    for (int i = 0; i < size; i++) {
        int idx = (h + i * i) % size;

        // Если встретили свободную ячейку — элемента нет
        if (table[idx].status == FREE)
            return;

        // Если нашли нужный элемент
        if (table[idx].status == ENGAGED &&
            !strcmp(table[idx].login, login)) {

            // Освобождаем память под хеш пароля
            delete[] table[idx].pass_hash;

            // Помечаем ячейку как удалённую
            table[idx].status = DELETED;

            count--;           // уменьшаем число элементов
            deleted_count++;   // увеличиваем число удалённых
            return;
        }
    }
}

// ----------------------
// Проверка необходимости пересборки таблицы
// ----------------------
void HashTable::rebuild_if_needed() {
    // Если таблица не перегружена и удалённых меньше, чем активных — ничего не делаем
    if (count * 2 < size && deleted_count < count)
        return;

    // Иначе выполняем resize
    resize();
}

// ----------------------
// Пересборка хеш-таблицы
// Увеличивает размер и переносит все элементы
// ----------------------
void HashTable::resize() {
    Pair* old = table;
    int oldSize = size;

    // Увеличиваем размер таблицы в два раза
    size *= 2;
    table = new Pair[size];

    count = 0;
    deleted_count = 0;

    // Перехешируем все активные элементы
    for (int i = 0; i < oldSize; i++) {
        if (old[i].status == ENGAGED) {
            add(old[i].login, old[i].pass_hash);
        }
    }

    // Удаляем старый массив (без удаления хешей!)
    delete[] old;
}