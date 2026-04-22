#ifndef HASH_TABLE
#define HASH_TABLE

#include "hash_functions.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <immintrin.h>

#define HASHT_DUMP(hash_table)\
        HashTableDump(hash_table,__FILE__, __LINE__)

#define ASSERT(hash_table) \
    assert(hash_table != NULL);\
    assert(hash_table->buckets != NULL);\
    assert(hash_table->capacity >= 0);\
    assert(hash_table->HashFunction.hash_f != NULL);\
    assert(hash_table->HashFunction.hf_name != NULL);\

const ssize_t HT_CAPASITY = 4001;

typedef struct
{
    size_t num_of_bytes_in_file;
    char *buffer;
} data_of_buffer;

typedef struct
{
    size_t num_of_words_in_file;
    char **pointers_on_words;
} words_info;

typedef struct Node 
{
    char *word_ptr;     
    struct Node *next;  
} Node;

typedef struct {
    Node *head;    
    size_t size;   
} HashBucket;

typedef struct
{
    UsedHashFunction hash_f;
    const char *hf_name;
} Using_HF;

Using_HF hf_structures[] = 
{
    {ZeroHF, "ZeroHF"},
    {FirstAlphaHF, "FirstAlphaHF"},
    {WordLengthHF, "WordLengthHF"},
    {ASCIIHF, "ASCIIHF"},
    {RolHF, "RolHF"},
    {CRC32HF, "CRC32HF"},
};

typedef struct
{
    ssize_t capacity;
    ssize_t total_elements;
    HashBucket *buckets; 
    Using_HF HashFunction;
    const char *file_name;
} hash_table_struct;

enum HashT_Errors
{
    NO_HT_ERROR,
    EMPTY_POINTER_ON_HT_STRUCTURE,
    EMPTY_POINTER_ON_DATA,
    EMPTY_POINTER_ON_HASH_FUNCTION,
    MISSING_HASH_FUNCTION_NAME,
    ERROR_IN_CAPACITY,
    ALLOCATE_MEMORY_ERROR,
    LIST_EXISTS_BUT_SIZE_ZERO,
    SIZE_EXISTS_BUT_LIST_EMPTY,
    FILE_OPENING_ERROR,
    FILE_CLOSING_ERROR
};

HashT_Errors HashTableInit(hash_table_struct *hash_table, ssize_t capacity, const char *logfile_name, Using_HF hash_function);
HashT_Errors HashTableVerify(hash_table_struct *hash_table);
HashT_Errors HashTableDestroy(hash_table_struct *hash_table);
void HashTableDump(const hash_table_struct *hash_table, const char *file, int line);
bool print_error(HashT_Errors err);

HashT_Errors FillInHashTable(hash_table_struct *hash_table, words_info words);
bool FindTheWordInHashTable(hash_table_struct *hash_table, const char *word);
HashT_Errors SaveDataToCSVFile(hash_table_struct *hash_table, const char *name_of_file);
char *GenerateHashHistogram(const char *csv_name) ;
HashT_Errors WriteToMDFile(hash_table_struct *hash_table, const char *png_name, const char *md_file_name);
ssize_t GetTotalElementsNumber(const hash_table_struct *hash_table);
double CalculateVariance(hash_table_struct *hash_table, size_t total_elements);

size_t GetNumOfBytesInFile(int fd1);
data_of_buffer ReadToBufferFromFile(const char *name_of_file);
size_t GetNumOfWordsInFile(char *buffer);
void GetPointersOnWords(char *buffer, char **pointers_on_words, size_t num_of_words_in_file);
words_info GetWordsStruct(char *buffer);
bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);

#endif