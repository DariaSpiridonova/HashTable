#include "hash_functions.h"

size_t ZeroHF(char *key, ssize_t capacity)
{
    return 0;
}

size_t FirstAlphaHF(char *key, ssize_t capacity)
{
    return key[0] % capacity;
}

size_t WordLengthHF(char *key, ssize_t capacity)
{
    return strlen(key) % capacity;
}

size_t ASCIIHF(char *key, ssize_t capacity)
{
    size_t ascii_sum = 0;
    for (size_t i = 0; i < strlen(key); i++)
    {
        ascii_sum += key[i];
    }

    return ascii_sum % capacity;
}

size_t RolHF(char *key, ssize_t capacity)
{
    size_t hash = 0;
    size_t bits = sizeof(size_t) * 8;
    size_t shift = 5;

    unsigned char *word = (unsigned char *)key;

    while (*word)
    {
        hash = (hash << shift) | (hash >> (bits - shift));

        hash = hash ^ (size_t)(*word);

        word++;
    }

    return hash % capacity;
}

//   1101000  (Сообщение с нулями)
// ^ 1011     (Делитель)
//   ---------
//   01100    (Результат первого XOR, сносим 0)
//   ^1011   (Снова применяем делитель)
//     -------
//    01110  (Сносим следующий 0)
//    ^1011
//       -----
//     0101 (Это остаток! Это и есть наш CRC)

uint32_t crc32_table[256];

void InitCRC32Table() 
{
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}

size_t CRC32HF(char *key, ssize_t capacity)
{
    unsigned char *word = (unsigned char *)key;

    uint32_t crc = ~0u;
    const uint32_t filter = 0xEDB88320;

    size_t i = 0;
    while (word[i] != '\0') 
    {
        crc ^= (uint32_t) word[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) 
            {
                crc = (crc >> 1) ^ filter;
            }
            else 
            {
                crc >>= 1;
            }
        }
        i++;
    }

    return (size_t)(~crc % capacity);
}