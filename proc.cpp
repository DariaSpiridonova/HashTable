#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    printf("До развилки (fork): я один, мой PID %d\n", getpid());
    printf("------------------------------------------\n");

    // printf("hello");
    // fflush(stdout);
    
    // В этот момент создается полная копия процесса
    pid = fork();

    if (pid < 0) {
        // Если fork вернул минус, значит произошел сбой (не хватило памяти)
        fprintf(stderr, "Ошибка при создании процесса\n");
        return 1;
    } 
    else if (pid == 0) {
        // Этот блок выполнит только РЕБЕНОК
        // fork() возвращает 0 внутри дочернего процесса
        printf("[Ребенок] Привет! Мой личный PID: %d\n", getpid());
        printf("[Ребенок] Мой папа имеет PID: %d\n", getppid());
    } 
    else {
        // Этот блок выполнит только РОДИТЕЛЬ
        // fork() возвращает PID созданного ребенка
        printf("[Родитель] Я создал ребенка с PID: %d\n", pid);
        printf("[Родитель] Мой собственный PID всё еще: %d\n", getpid());
    }

    printf("После развилки: процесс с PID %d заканчивает работу\n", getpid());

    getchar();

    printf("hello");
    fflush(stdout);

    sleep(5);

    return 0;
}
