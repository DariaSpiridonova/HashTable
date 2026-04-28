#include "hash_table.h" 

HashT_Errors FillInHashTable(hash_table_struct *hash_table, words_info words)
{
    ASSERT(hash_table);

    size_t hash_index = 0;
    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    printf("num_of_words_in_file = %zu\n", words.num_of_words_in_file);

    UsedHashFunction using_hf = hash_table->HashFunction.hash_f;
    unsigned long long start = __rdtsc();
    if (using_hf == CRC32HF)
    {
        InitCRC32Table();
    }
    for (size_t i = 0; i < words.num_of_words_in_file; i++)
    {
        hash_index = using_hf(&(words.pointers_on_words_structures[i]), hash_table->capacity);
        
        Node *node = hash_table->buckets[hash_index].head; 
        
        while (node != NULL) 
        {
            #ifdef _MY_STRCMP
            if (!my_strcmp(&(words.pointers_on_words_structures[i]), &(node->str_node))) 
            #else 
            if (!strcmp(words.pointers_on_words_structures[i].str, node->str_node.str))
            #endif
            {
                break; // found duplicate
            }
            node = node->next;
        }

        if (node == NULL) 
        {
            Node *new_node = &(hash_table->nodes_pool[hash_table->next_free_node++]);
               
            new_node->str_node = words.pointers_on_words_structures[i];
            new_node->next = hash_table->buckets[hash_index].head;
            
            hash_table->buckets[hash_index].head = new_node;
            hash_table->buckets[hash_index].size++;
        }
    }
    unsigned long long end = __rdtsc();
    printf("Ticks for cycle in FillInHashTable: %llu\n", end - start);

    hash_table->total_elements = GetTotalElementsNumber(hash_table);

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    ASSERT(hash_table);

    return err;
}

bool FindTheWordInHashTable(hash_table_struct *hash_table, String_Node *word_structure)
{
    size_t hash_index = 0;

    hash_index = hash_table->HashFunction.hash_f(word_structure, hash_table->capacity);

    Node *node = hash_table->buckets[hash_index].head;

    while (node != NULL)
    {
        int is_len_equal = 0;

        // Compare lengths
        asm volatile (
            "mov 8(%[node_str]), %%eax \n\t"  
            "cmp %[target_len], %%eax  \n\t"  
            "jne 1f                    \n\t"
            "mov $1, %[res]            \n\t"  // if equal, res = 1
            "1:                        \n\t"
            : [res] "=r" (is_len_equal)
            : [node_str] "r" (&node->str_node), [target_len] "r" (word_structure->len)
            : "eax", "cc"
        );

        if (is_len_equal) 
        {
            if (!my_strcmp(word_structure, &(node->str_node)))
            {
                return true;
            }
        }
        node = node->next;
    }
    
    return false;
}

HashT_Errors SaveDataToCSVFile(hash_table_struct *hash_table, const char *name_of_file)
{
    ASSERT(hash_table);

    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    FILE *csv_file = fopen(name_of_file, "w");
    
    if (!OpenFileSuccess(csv_file, name_of_file))
        return FILE_OPENING_ERROR;

    fprintf(csv_file, "Bucket_Index;Words_Count\n");

    for (size_t i = 0; i < hash_table->capacity; i++)
    {
        fprintf(csv_file, "%zu;%zu\n", i, hash_table->buckets[i].size);
    }

    if (!CloseFileSuccess(csv_file, name_of_file))
        return FILE_CLOSING_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    ASSERT(hash_table);

    return err;
}

// Hash Function: 
// Table Capacity: 
// Total Words: 
// Variance: 
// и ГИСТОГРАММА ОБЯЗАТЕЛЬНО!!!

HashT_Errors WriteToMDFile(hash_table_struct *hash_table, const char *png_name, const char *md_file_name)
{
    ASSERT(hash_table);

    HashT_Errors err = NO_HT_ERROR;
    
    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    FILE *md_file = fopen(md_file_name, "a");
    
    if (!OpenFileSuccess(md_file, md_file_name))
        return FILE_OPENING_ERROR;

    double variance = CalculateVariance(hash_table, hash_table->total_elements);

    fprintf(md_file, "## RESULTS\n\n");
    
    fprintf(md_file, "**Hash Function      :** %s \n", hash_table->HashFunction.hf_name);
    fprintf(md_file, "**Hash Table Capacity:** %zd\n", hash_table->capacity);
    fprintf(md_file, "**Total Words        :** %zd\n", hash_table->total_elements);
    fprintf(md_file, "**Variance           :** %lf\n", variance);

    fprintf(md_file, "### Hash Histogram\n");
    fprintf(md_file, "![A histogram of the distribution of words in a hash table](%s)\n\n", png_name);
    fprintf(md_file, "-------------------------------------------------------------------------\n\n"); 

    if (!CloseFileSuccess(md_file, md_file_name))
        return FILE_CLOSING_ERROR;

    if ((err = HashTableVerify(hash_table)))
    {
        HASHT_DUMP(hash_table);
        return err;
    }

    ASSERT(hash_table);

    return err;
}

ssize_t GetTotalElementsNumber(const hash_table_struct *hash_table)
{
    ssize_t sum = 0;
    for (size_t i = 0; i < hash_table->capacity; i++)
    {
        sum += hash_table->buckets[i].size;
    }

    return sum;
}

double CalculateVariance(hash_table_struct *hash_table, size_t total_elements)
{
    double avg = total_elements / hash_table->capacity;
    double sum = 0;
    for (size_t i = 0; i < hash_table->capacity; i++) 
    {
        double diff = (double)hash_table->buckets[i].size - avg;
        sum += diff * diff;
    }

    return sum / hash_table->capacity;
}

char *GenerateHashHistogram(const char *csv_name) 
{
    assert(csv_name != NULL);

    char png_name[256] = {0};

    time_t rawtime;      
    struct tm *timeinfo; 
    char buffer[64];

    // Get the current calendar time
    time(&rawtime);

    // Convert the calendar time to local time
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);

    // Print the formatted time string
    printf("Current time: %s\n", buffer);
    struct timespec tstart={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    char *dot_position = strrchr((char *)csv_name, '.');
    char *base_csv_name = NULL;

    if (dot_position) 
    {
        base_csv_name = strndup(csv_name, dot_position - csv_name);
    } 
    else 
    {
        base_csv_name = strdup(csv_name);
    }

    snprintf(png_name, sizeof(png_name), "%s_%s%ld.png", base_csv_name, buffer, tstart.tv_nsec);
    free(base_csv_name);
    
    FILE *gnupipe = popen("gnuplot -persistent", "w");
    if (!gnupipe) 
    {
        fprintf(stderr, "Gnuplot not found. Please install it.\n");
        return NULL;
    }

    fprintf(gnupipe, "set terminal pngcairo size 800,600\n");  // Формат и размер
    fprintf(gnupipe, "set output '%s'\n", png_name);           // Имя выходного файла
    fprintf(gnupipe, "set datafile separator ';'\n");         
    
    fprintf(gnupipe, "set title 'Hash Table Distribution'\n");
    fprintf(gnupipe, "set xlabel 'Bucket Index'\n");
    fprintf(gnupipe, "set ylabel 'Words Count'\n");
    
    fprintf(gnupipe, "set style fill solid 0.5\n");
    fprintf(gnupipe, "set boxwidth 0.8 relative\n");
    
    fprintf(gnupipe, "plot '%s' using 1:2 with boxes title 'Load'\n", csv_name);

    pclose(gnupipe);

    return strdup(png_name); 
}
