#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define BOLD_BEGIN "\033[1m"
#define BOLD_END   "\033[0m"

typedef size_t (*UsedHashFunction)(char *key, ssize_t capacity);

enum HashFunctions
{
    ZERO_HF = 0,
    FIRST_ALPHA_HF = 1,
    WORD_LENGTH_HF = 2,
    ASCII_HF = 3,
    ROl_HF = 4,
    CRC32_HF = 5,
    HF_COUNT = 6
};
const size_t NUMBER_OF_HF = HF_COUNT;

const char *const hash_functions_names[NUMBER_OF_HF] = {"ZeroHF", "FirstAlphaHF", "WordLengthHF", "ASCIIHF", "RolHF", "CRC32HF"};
// TODO: structure with name and function for accepting by initializator

size_t ZeroHF(char *key, ssize_t capacity);
size_t FirstAlphaHF(char *key, ssize_t capacity);
size_t WordLengthHF(char *key, ssize_t capacity);
size_t ASCIIHF(char *key, ssize_t capacity);
size_t RolHF(char *key, ssize_t capacity);
size_t CRC32HF(char *key, ssize_t capacity);

const UsedHashFunction hash_functions[NUMBER_OF_HF] = 
{
    ZeroHF,
    FirstAlphaHF,
    WordLengthHF,
    ASCIIHF,
    RolHF,
    CRC32HF
};

void InitCRC32Table(void);

#endif 
