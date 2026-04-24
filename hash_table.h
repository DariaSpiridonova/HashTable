#ifndef HASH_TABLE
#define HASH_TABLE

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <immintrin.h>
#include <stdalign.h>

#define HASHT_DUMP(hash_table)\
        HashTableDump(hash_table,__FILE__, __LINE__)

#define ASSERT(hash_table) \
    assert(hash_table != NULL);\
    assert(hash_table->buckets != NULL);\
    assert(hash_table->nodes_pool != NULL);\
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
    char *str;
    uint32_t len;
} String_Node;

typedef struct
{
    size_t num_of_words_in_file;
    String_Node *pointers_on_words_structures;
} words_info;

typedef struct alignas(32) Node 
{
    String_Node str_node;     
    struct Node *next;  
} Node;

typedef struct {
    Node *head;    
    size_t size;   
} HashBucket;

typedef size_t (*UsedHashFunction)(String_Node *str_node, ssize_t capacity);

typedef struct
{
    UsedHashFunction hash_f;
    const char *hf_name;
} Using_HF;

typedef struct
{
    ssize_t capacity;
    ssize_t total_elements;
    HashBucket *buckets; 
    Using_HF HashFunction;
    const char *file_name;

    Node *nodes_pool;      
    size_t next_free_node; 
} hash_table_struct;

enum HashT_Errors
{
    NO_HT_ERROR,
    EMPTY_POINTER_ON_HT_STRUCTURE,
    EMPTY_POINTER_ON_DATA,
    EMPTY_POINTER_ON_HASH_FUNCTION,
    MISSING_HASH_FUNCTION_NAME,
    EMPTY_POINTER_ON_NODES_POOL,
    ERROR_IN_CAPACITY,
    ALLOCATE_MEMORY_ERROR,
    LIST_EXISTS_BUT_SIZE_ZERO,
    SIZE_EXISTS_BUT_LIST_EMPTY,
    FILE_OPENING_ERROR,
    FILE_CLOSING_ERROR
};

HashT_Errors HashTableInit(hash_table_struct *hash_table, size_t num_of_words_in_file, ssize_t capacity, const char *logfile_name, Using_HF hash_function);
HashT_Errors HashTableVerify(hash_table_struct *hash_table);
HashT_Errors HashTableDestroy(hash_table_struct *hash_table);
void HashTableDump(const hash_table_struct *hash_table, const char *file, int line);
bool print_error(HashT_Errors err);

HashT_Errors FillInHashTable(hash_table_struct *hash_table, words_info words);
bool FindTheWordInHashTable(hash_table_struct *hash_table, String_Node *word_structure);
HashT_Errors SaveDataToCSVFile(hash_table_struct *hash_table, const char *name_of_file);
char *GenerateHashHistogram(const char *csv_name) ;
HashT_Errors WriteToMDFile(hash_table_struct *hash_table, const char *png_name, const char *md_file_name);
ssize_t GetTotalElementsNumber(const hash_table_struct *hash_table);
double CalculateVariance(hash_table_struct *hash_table, size_t total_elements);

size_t GetNumOfBytesInFile(int fd1);
data_of_buffer ReadToBufferFromFile(const char *name_of_file);
size_t GetNumOfWordsInFile(char *buffer);
void GetPointersOnWords(char *buffer, String_Node *words_structures, size_t num_of_words_in_file);
__attribute__((noinline))
words_info GetWordsStruct(char *buffer);
bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);


size_t ZeroHF       (String_Node *str_node, ssize_t capacity);
size_t FirstAlphaHF (String_Node *str_node, ssize_t capacity);
size_t WordLengthHF (String_Node *str_node, ssize_t capacity);
size_t ASCIIHF      (String_Node *str_node, ssize_t capacity);
size_t RolHF        (String_Node *str_node, ssize_t capacity);
size_t CRC32HF      (String_Node *str_node, ssize_t capacity);
size_t CRC32HFIntrin(String_Node *str_node, ssize_t capacity);

void InitCRC32Table(void);

#endif