#include "hash_table.h"

size_t ZeroHF([[maybe_unused]] String_Node *str_node, [[maybe_unused]] ssize_t capacity)
{
    return 0;
}

size_t FirstAlphaHF(String_Node *str_node, ssize_t capacity)
{
    return str_node->str[0] % capacity;
}

size_t WordLengthHF(String_Node *str_node, ssize_t capacity)
{
    return strlen(str_node->str) % capacity;
}

size_t ASCIIHF(String_Node *str_node, ssize_t capacity)
{
    size_t ascii_sum = 0;
    for (size_t i = 0; i < strlen(str_node->str); i++)
    {
        ascii_sum += str_node->str[i];
    }

    return ascii_sum % capacity;
}

size_t RolHF(String_Node *str_node, ssize_t capacity)
{
    size_t hash = 0;
    size_t bits = sizeof(size_t) * 8;
    size_t shift = 5;

    unsigned char *word = (unsigned char *)str_node->str;

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

// size_t CRC32HF(String_Node *str_node, ssize_t capacity)
// {
//     unsigned char *word = (unsigned char *)str_node->str;

//     uint32_t crc = ~0u;
//     const uint32_t filter = 0xEDB88320;

//     size_t i = 0;
//     while (word[i] != '\0') 
//     {
//         crc ^= (uint32_t) word[i];
//         for (int j = 0; j < 8; j++) 
//         {
//             if (crc & 1) 
//             {
//                 crc = (crc >> 1) ^ filter;
//             }
//             else 
//             {
//                 crc >>= 1;
//             }
//         }
//         i++;
//     }

//     return (size_t)(~crc % capacity);
// }

size_t CRC32HF(String_Node *str_node, ssize_t capacity)
{
    uint32_t crc = 0xFFFFFFFF;
    unsigned char *word = (unsigned char *)str_node->str;

    while (*word) 
    {
        crc = (crc >> 8) ^ crc32_table[(crc ^ *word) & 0xFF];
        word++;
    }

    uint32_t final_crc = crc ^ 0xFFFFFFFF;
    
    return (size_t)final_crc % capacity;
}

size_t CRC32HFIntrin(String_Node *str_node, ssize_t capacity) 
{
    uint64_t crc = 0xFFFFFFFF;
    size_t i = 0;

    // Шаг 1: Обработка блоками по 8 байт
    for (; i + 8 <= str_node->len; i += 8) 
    {
        crc = _mm_crc32_u64(crc, *(uint64_t*)(str_node->str + i));
    }

    // Шаг 2: Дообработка хвоста (если длина не кратна 8)
    if (i + 4 <= str_node->len) 
    {
        crc = _mm_crc32_u32((uint32_t)crc, *(uint32_t*)(str_node->str + i));
        i += 4;
    }

    while (i < str_node->len) 
    {
        crc = _mm_crc32_u8((uint32_t)crc, str_node->str[i++]);
    }

    return (size_t)(crc ^ 0xFFFFFFFF) % capacity;
}
