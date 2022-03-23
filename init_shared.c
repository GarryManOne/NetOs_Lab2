#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "lab2.h"

// Главная функция
int main(int argc, char * argv[])
{
    int shm = 0;    //  Дескриптора объекта разделяемой памяти

    // Открываем объект разделяемой памяти
    if( (shm = shm_open (SHARED_MEMORY_OBJECT_NAME, O_CREAT|O_RDWR, 0777)) == -1){
        perror("shm_open");
        return -1;
    }

    ftruncate(shm, sizeof(shr_mem)); 

    // Создаем отображение
    shr_mem* memory = (shr_mem*)mmap(NULL, sizeof(shr_mem), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    if (memory == (shr_mem*)-1){
        perror("mmap");
        return 1;
    }
    
    // Закрываем объект разделяемой памяти
    close(shm);


    // Обнуление карты 
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            memory->map[i][j] = 17;
        }
    }

    for(int i = 0; i < kMapSizeX*kMapSizeY; i++){
    memory->db_animals[i].type = NONE;
    }
    
    return 0;
}
