#include "hash_table.h"
#include <sys/stat.h>

HashT_Errors AlignTheBuffer(data_of_buffer *data_in_buffer, words_info *words)
{
    assert(data_in_buffer != NULL);
    assert(words != NULL);

    size_t total_size = words->num_of_words_in_file * 32;
    
    char *aligned_buffer = (char *)aligned_alloc(32, total_size);
    if (aligned_buffer == NULL) 
        return ALLOCATE_MEMORY_ERROR;

    char *aligned_buffer_begin = aligned_buffer;

    memset(aligned_buffer, 0, total_size);
    
    for (size_t i = 0; i < words->num_of_words_in_file; i++)
    {
        size_t len = words->pointers_on_words_structures[i].len;
        memcpy(aligned_buffer, words->pointers_on_words_structures[i].str, len);
        words->pointers_on_words_structures[i].str = aligned_buffer;

        aligned_buffer += 32;
    }

    free(data_in_buffer->buffer);
    data_in_buffer->buffer = aligned_buffer_begin;

    return NO_HT_ERROR;
}

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

// size_t GetNumOfWordsInFile1(char *buffer)
// {
//     assert(buffer != NULL);
//     size_t num = 0;
    
//     asm volatile (
//         "xor %%rcx, %%rcx\n\t"       // num = 0
//         "xor %%rbx, %%rbx\n\t"       // inside_word = 0

//         "1:\n\t"                     // Метка 1 (начало цикла)
//         "movzx (%%rdi), %%eax\n\t"
//         "test %%al, %%al\n\t"
//         "jz 4f\n\t"                  // Прыжок вперед (f) к метке 4 (конец)

//         "and $~0x20, %%al\n\t"
//         "sub $65, %%al\n\t"
//         "cmp $25, %%al\n\t"
//         "ja 2f\n\t"                  // Прыжок вперед к метке 2 (not_alpha)

//         "test %%rbx, %%rbx\n\t"
//         "jnz 3f\n\t"                 // Прыжок вперед к метке 3 (next_byte)
//         "inc %%rcx\n\t"
//         "mov $1, %%rbx\n\t"
//         "jmp 3f\n\t"

//         "2:\n\t"                     // Метка 2 (not_alpha)
//         "xor %%rbx, %%rbx\n\t"
//         "movb $0, (%%rdi)\n\t"       // Зануляем разделитель, как в твоем коде

//         "3:\n\t"                     // Метка 3 (next_byte)
//         "inc %%rdi\n\t"
//         "jmp 1b\n\t"                 // Прыжок назад (b) к метке 1 (начало)

//         "4:\n\t"                     // Метка 4 (конец)
//         "mov %%rcx, %[num]\n\t"
        
//         : [num] "=r" (num)
//         : "D" (buffer)
//         : "rax", "rbx", "rcx", "memory" // Добавил memory, т.к. мы меняем буфер
//     );

//     return num;
// }

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

    unsigned long long start = __rdtsc();
    size_t num_of_words_in_file = GetNumOfWordsInFile(buffer);
    unsigned long long end = __rdtsc();
    printf("Ticks for GetNumOfWordsInFile: %llu\n", end - start);

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
