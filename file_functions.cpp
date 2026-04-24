#include "hash_table.h"
#include <sys/stat.h>

size_t GetNumOfBytesInFile(int fd1)
{
    struct stat st1 = {};
    fstat(fd1, &st1);

    return (size_t)st1.st_size;
}

data_of_buffer ReadToBufferFromFile(const char *name_of_file)
{
    FILE *text = fopen(name_of_file, "rb");
    
    if (!OpenFileSuccess(text, name_of_file))
        return {0, 0};

    data_of_buffer data = {0, 0};

    size_t num_of_bytes_in_file = GetNumOfBytesInFile(fileno(text));

    // printf("%d\n", num_of_bytes_in_file); // проверка количества символов в файле

    char *buffer = (char *)calloc(num_of_bytes_in_file + 1, sizeof(char));
    if (buffer == NULL)
        return {0, 0};

    size_t num_success_read_symbols = fread(buffer, sizeof(char), num_of_bytes_in_file, text);
    if (num_success_read_symbols < num_of_bytes_in_file)
        return {0, 0};

    if (!CloseFileSuccess(text, name_of_file))
        return {0, 0};

    // printf("%d\n", num_success_read_symbols); // проверка количества успешно прочитанных в буфер символов

    buffer[num_success_read_symbols] = '\0';
    // printf("%s\n", buffer); // проверка содержимого буфера

    data.num_of_bytes_in_file = num_of_bytes_in_file;
    data.buffer = buffer;

    return data;
}

size_t GetNumOfWordsInFile(char *buffer)
{
    assert(buffer != NULL);

    char *buffer_begin = buffer;
    size_t num = 0;
    char *point_on_new_word = NULL;
    bool inside_word = false;
    
    while (*buffer != '\0')
    {
        if (isalpha(*buffer))
        {
            tolower(*buffer);
            if (!inside_word)
            {
                inside_word = true;
                num++;
            }
        }
        else if (inside_word)
        {
            *buffer = '\0';
            inside_word = false;
        }

        buffer++;
    }

    return num;
}

void GetPointersOnWords(char *buffer, String_Node *words_structures, size_t num_of_words_in_file)
{
    assert(words_structures != NULL);
    assert(buffer != NULL);

    while (!isalpha(*buffer))
    {
        buffer++;
    }

    char *ptr_on_word = buffer;
    char *pointer_on_new_word = NULL;

    size_t i = 0;
    for (; i < num_of_words_in_file - 1; i++)
    {
        words_structures[i].str = ptr_on_word;
        words_structures[i].len = strlen(ptr_on_word);
        ptr_on_word += words_structures[i].len + 1;
        while (!isalpha(*ptr_on_word))
            ptr_on_word++;
    }
    words_structures[i].str = ptr_on_word;
    words_structures[i].len = strlen(ptr_on_word);
}

__attribute__((noinline))
words_info GetWordsStruct(char *buffer)
{
    assert(buffer != NULL);

    words_info words = {0, NULL};
    size_t num_of_words_in_file = GetNumOfWordsInFile(buffer);
    // printf("%d\n", GetNumOfWordsInFile); // проверка количества слов в файле

    String_Node *words_structures = (String_Node *)calloc(num_of_words_in_file, sizeof(String_Node));
    GetPointersOnWords(buffer, words_structures, num_of_words_in_file);

    words.num_of_words_in_file = num_of_words_in_file;
    words.pointers_on_words_structures = words_structures;

    return words;
}

bool OpenFileSuccess(FILE *fp, const char * file_name)
{
    if (fp == NULL)
    {
        printf("An error occurred when opening the file %s", file_name);
        return false;
    }

    return true;
}

bool CloseFileSuccess(FILE *fp, const char * file_name)
{
    if (fclose(fp))
    {
        printf("An error occurred when closing the file %s", file_name);
        return false;
    }
    
    return true;
}
