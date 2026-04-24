#include "hash_table.h"

HashT_Errors HashTableInit(hash_table_struct *hash_table, size_t num_of_words_in_file, ssize_t capacity, const char *logfile_name, Using_HF hash_function)
{
    assert(hash_table != NULL);
    assert(capacity > 0);

    hash_table->buckets = (HashBucket *)calloc(capacity, sizeof(HashBucket));
    if (hash_table->buckets == NULL) 
        return ALLOCATE_MEMORY_ERROR;
    
    size_t pool_size = sizeof(Node) * (size_t(num_of_words_in_file / 2) * 2 + 2);
    hash_table->nodes_pool = (Node *)aligned_alloc(64, pool_size);
    if (hash_table->nodes_pool == NULL) 
    {
        free(hash_table->buckets);
        return ALLOCATE_MEMORY_ERROR;
    }
    memset(hash_table->nodes_pool, 0, pool_size);
    
    hash_table->file_name = logfile_name;
    hash_table->capacity = capacity;
    hash_table->total_elements = 0;
    hash_table->next_free_node = 0;
    hash_table->HashFunction = hash_function;

    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        free(hash_table->buckets);
        free(hash_table->nodes_pool);
        HASHT_DUMP(hash_table);
        return err;
    }

    ASSERT(hash_table);

    return err;
}

HashT_Errors HashTableVerify(hash_table_struct *hash_table)
{
    ASSERT(hash_table);

    if      (hash_table == NULL)                       return EMPTY_POINTER_ON_HT_STRUCTURE;
    if      (hash_table->capacity < 0)                 return ERROR_IN_CAPACITY;
    if      (hash_table->buckets == NULL)              return EMPTY_POINTER_ON_DATA;
    if      (hash_table->HashFunction.hash_f == NULL)  return EMPTY_POINTER_ON_HASH_FUNCTION;
    if      (hash_table->HashFunction.hf_name == NULL) return MISSING_HASH_FUNCTION_NAME;
    if      (hash_table->nodes_pool == NULL)           return EMPTY_POINTER_ON_NODES_POOL;
    
    for (size_t i = 0; i < hash_table->capacity; i++)
    {
        if (hash_table->buckets[i].head != NULL && hash_table->buckets[i].size == 0)
        {
            return LIST_EXISTS_BUT_SIZE_ZERO; 
        }

        if (hash_table->buckets[i].size > 0     && hash_table->buckets[i].head == NULL)
        {
            return SIZE_EXISTS_BUT_LIST_EMPTY; 
        }
    }

    ASSERT(hash_table);

    return NO_HT_ERROR;
}

HashT_Errors HashTableDestroy(hash_table_struct *hash_table)
{
    ASSERT(hash_table);

    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    free(hash_table->buckets);
    free(hash_table->nodes_pool);

    hash_table->buckets = NULL;
    hash_table->nodes_pool = NULL;
    hash_table->capacity = 0;
    hash_table->total_elements = 0;
    hash_table->next_free_node = 0;
    
    printf("FREE\n");

    return err;
}

void HashTableDump(const hash_table_struct *hash_table, const char *file, int line)
{
    ASSERT(hash_table);

    // dump_to_console(stdin, hash_table, file, line);

    time_t rawtime;      
    struct tm *timeinfo; 
    char buffer[80];

    // Get the current calendar time
    time(&rawtime);

    // Convert the calendar time to local time
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);

    // Print the formatted time string
    printf("Current time: %s\n", buffer);
    struct timespec tstart={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    // char gvfile_name[SIZE_OF_NAME] = {0};
    // sprintf(gvfile_name, "%s%s%s%ld.gv", link_to_graphviz_file, hash_table->file_name, buffer, tstart.tv_nsec);
    // printf("%s%s%s%ld.gv\n", link_to_graphviz_file, hash_table->file_name, buffer, tstart.tv_nsec);

    // dump_to_logfile(hash_table, hash_table->file_name, gvfile_name);

    // create_graph(hash_table, gvfile_name);

    ASSERT(hash_table);
}

bool print_error(HashT_Errors err)
{
    switch(err)
    {
        case NO_HT_ERROR:
            return true;

        case EMPTY_POINTER_ON_HT_STRUCTURE:
            printf("The pointer to the HASH TABLE is empty");
            return true;

        case EMPTY_POINTER_ON_DATA:
            printf("The pointer to the DATA is empty");
            return true;

        case EMPTY_POINTER_ON_HASH_FUNCTION:
            printf("The pointer to the HASH FUNCTION is empty");
            return true;

        case ERROR_IN_CAPACITY:
            printf("Capacity takes an invalid value");
            return true;

        case ALLOCATE_MEMORY_ERROR:
            printf("Couldn't allocate additional memory");
            return true;

        case FILE_OPENING_ERROR:
            printf("An error occurred when opening the file");
            return true;

        case FILE_CLOSING_ERROR:
            printf("An error occurred when closing the file");
            return true;

        default:
            return false;
    }

    return false;
}