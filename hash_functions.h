#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "hash_table.h"

#define BOLD_BEGIN "\033[1m"
#define BOLD_END   "\033[0m"

typedef size_t (*UsedHashFunction)(String_Node str_node, ssize_t capacity);

size_t ZeroHF(String_Node str_node, ssize_t capacity);
size_t FirstAlphaHF(String_Node str_node, ssize_t capacity);
size_t WordLengthHF(String_Node str_node, ssize_t capacity);
size_t ASCIIHF(String_Node str_node, ssize_t capacity);
size_t RolHF(String_Node str_node, ssize_t capacity);
size_t CRC32HF(String_Node str_node, ssize_t capacity);
size_t CRC32HFIntrin(String_Node str_node, ssize_t capacity);

void InitCRC32Table(void);

#endif 
