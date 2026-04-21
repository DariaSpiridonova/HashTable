#include "hash_table.h"
#include "hash_functions.h"

HashT_Errors HashTableInit(hash_table_struct *hash_table, ssize_t capacity, const char *logfile_name, HashFunctions hash_function)
{
    assert(hash_table != NULL);
    assert(capacity > 0);

    hash_table->file_name = logfile_name;

    hash_table->buckets = (HashBucket *)calloc(capacity, sizeof(HashBucket));
    if (hash_table->buckets == NULL) 
        return ALLOCATE_MEMORY_ERROR;

    hash_table->capacity = capacity;
    hash_table->total_elements = 0;
    hash_table->HashFunction = hash_function;

    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    ASSERT(hash_table);

    return err;
}

HashT_Errors HashTableVerify(hash_table_struct *hash_table)
{
    ASSERT(hash_table);

    if      (hash_table == NULL)                      return EMPTY_POINTER_ON_HT_STRUCTURE;
    else if (hash_table->capacity < 0)                return ERROR_IN_CAPACITY;
    else if (hash_table->buckets == NULL)             return EMPTY_POINTER_ON_DATA;
    if      (hash_table->HashFunction >= NUMBER_OF_HF) return EMPTY_POINTER_ON_HASH_FUNCTION;
    
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

    Node *current_node = NULL;
    Node *next_node = NULL;

    for (size_t i = 0; i < hash_table->capacity; i++)
    {
        current_node = hash_table->buckets[i].head;
        while (current_node != NULL)
        {
            next_node = current_node->next;
            free(current_node);
            current_node = next_node;
        }
    }
    
    free(hash_table->buckets);
    hash_table->buckets = NULL;
    hash_table->capacity = 0;
    hash_table->total_elements = 0;
    
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