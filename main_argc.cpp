#include "hash_table.h"

const Using_HF hf_structures[] = 
{
    {ZeroHF, "ZeroHF"},
    {FirstAlphaHF, "FirstAlphaHF"},
    {WordLengthHF, "WordLengthHF"},
    {ASCIIHF, "ASCIIHF"},
    {RolHF, "RolHF"},
    {CRC32HF, "CRC32HF"},
};

int main(int argc, char *argv[])
{  
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <function_number>\n", argv[0]);
        return 1;
    }

    Using_HF HashF = hf_structures[atoi(argv[1])];
    
    data_of_buffer data_in_buffer = ReadToBufferFromFile("text.txt");
    if (data_in_buffer.buffer == NULL || data_in_buffer.num_of_bytes_in_file == 0)
        return 1;

    words_info words = GetWordsStruct(data_in_buffer.buffer);
    if (words.num_of_words_in_file == 0 || words.pointers_on_words == NULL)
        return 2;

    hash_table_struct hash_table;

    HashT_Errors err = NO_HT_ERROR;

    if ((err = HashTableInit(&hash_table, 4001, "logfile.htm", HashF)))
    {
        print_error(err);
        return 3;
    }

    if ((err = FillInHashTable(&hash_table, words)))
    {
        print_error(err);
        return 4;
    }

    // Что мы оптимизируем? Поиск по хеш таблице!
    // Берем N слов и ищем их, замеряя время

    // hyperfine, rdtsc

    // if ((err = SaveDataToCSVFile(&hash_table, "hash_table.csv")))
    // {
    //     print_error(err);
    //     return 5;
    // }

    // const char *png_file_name = GenerateHashHistogram("hash_table.csv");

    // if ((err = WriteToMDFile(&hash_table, png_file_name, "hash_table.md")))
    // {
    //     print_error(err);
    //     return 6;
    // }

    volatile bool found = false;

    unsigned long long start = __rdtsc();
    for (size_t i = 0; i < words.num_of_words_in_file; i++)
    {
        found = FindTheWordInHashTable(&hash_table, words.pointers_on_words[i]);
    }
    unsigned long long end = __rdtsc();
    printf("Ticks: %llu\n", end - start);
    
    if ((err = HashTableDestroy(&hash_table)))
    {
        print_error(err);
        return 7;
    }

    free(data_in_buffer.buffer);
    free(words.pointers_on_words);
    // free((void*)png_file_name); 

    return 0;
}
