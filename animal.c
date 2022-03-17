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


// Генерация псевдослучайных чисел на определнном промежутке
int GetRandRangeInt(int min, int max){
    return rand() % (max - min) + min;
}


// Вывод карты в консоль
void PrintMap(shr_mem* memory){
    // char* buf;
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            if (memory->map[i][j] == 17){
                // fprintf(log_file, "[*]");
                printf("[*]");
                // buf = (char*)"[*]";
                // write(memory->file_map, buf, 3);
            }
            else{
                // fprintf(log_file, "[%d]", db_animals[map[i][j]].type);
                printf("[%d]", memory->db_animals[memory->map[i][j]].type);
                // sprintf(buf,"[%d]", memory->db_animals[memory->map[i][j]].type);
                // write(memory->file_map, buf, 3);
            }                }
        // fprintf(log_file, "\n");
        printf("\n");
        // write(memory->file_map, "\n", 2);
    }
    // fprintf(log_file, "\n");
    printf("\n");
    // write(memory->file_map, "\n", 2);
}


// Главная функция
int main(int argc, char * argv[])
{
    srand(time(NULL));

    if(argc != 4)
    {
        printf("Не достаточно аргументов!");
    }

    int x = atoi(argv[1]);              //  Расположение по X
    int y = atoi(argv[2]);              //  Расположение по Y
    int type_animal = atoi(argv[3]);    //  Тип животного

    int shm = 0;                        //  Дескриптора объекта разделяемой памяти

    // Открываем объект разделяемой памяти
    if( (shm = shm_open (SHARED_MEMORY_OBJECT_NAME, O_CREAT|O_RDWR, 0777)) == -1){
        perror("shm_open");
        return -1;
    }

    // Создаем отображение
    shr_mem* memory = (shr_mem*)mmap(NULL, sizeof(shr_mem), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    if (memory == (shr_mem*)-1){
        perror("mmap");
        return 1;
    }
    
    // Закрываем объект разделяемой памяти
    close(shm);

    //  Создание и инициализация мьютексов
    pthread_mutex_init(&memory->mutex, NULL);    

    // Блокировка
    pthread_mutex_lock(&memory->mutex);

    int index = 0;

    // Расположение животного на карте
    if (memory->map[x][y] == 17){
        for(int i = 0; i < kMapSizeX*kMapSizeY; i++){
            if (memory->db_animals[i].type == NONE){
                memory->db_animals[i].type = type_animal;                   // Тип животного              
                memory->db_animals[i].coord.x = x;                          // Координаты по X
                memory->db_animals[i].coord.y = y;                          // Координаты по Y
                memory->db_animals[i].life_time = kLifeTime;                // Продолжительность жизни
                memory->db_animals[i].startvation_time = kStarvationTime;   // Продолжительность голодания
                memory->map[x][y] = i;
                index = i;
            }
        }
    }
    else{
        exit(0);
    }

    // Разблокировка
    pthread_mutex_unlock(&memory->mutex);

    // printf("%u-%d\n", getpid(),index);
    // printf("%x\n", memory);
    char childX[10];                // Координаты по X детеныша
    char childY[10];                // Координаты по Y детеныша
    char childTypeAnimal[10];       // Тип Детеныша

    while (1){
        // Смотрим продолжительность жизни 
        if (memory->db_animals[index].life_time == 0 || memory->db_animals[index].startvation_time == 0){
            memory->map[memory->db_animals[index].coord.x][memory->db_animals[index].coord.y] = 17;
            exit(0);
        }

        int x = memory->db_animals[index].coord.x;
        int y = memory->db_animals[index].coord.y;

        // Случайное передвижение
        switch (GetRandRangeInt(0, 4)){
            case RIGHT:
                if ((x + 1) < kMapSizeX){
                    x += 1;
                }
                else continue;
                break;
            case LEFT: 
                if ((x - 1) > 0){
                    x -= 1;
                }
                else continue;
                break;
            case UP: 
                if ((y - 1) > 0){
                    y -= 1;
                }
                else continue;
                break;
            case DOWN: 
                if ((y + 1) < kMapSizeY){
                    y += 1;
                }
                else continue;
                break;
            default:
                break;
        }

        pthread_mutex_lock(&memory->mutex);

        // Смотрим продолжительность жизни 
        if (memory->db_animals[index].life_time == 0){
            pthread_mutex_unlock(&memory->mutex);
            exit(0);
        }

        // printf("%u шаг=%d \n", getpid(), n++);

        // Проверка какое животное находится в этой ячейке
        if(memory->map[x][y] != 17){
            
            // Спаривание :)
            if (memory->db_animals[index].type == memory->db_animals[memory->map[x][y]].type){
                
                // Смотрим продолжительность жизни 
                if (memory->db_animals[index].life_time == 0){
                    pthread_mutex_unlock(&memory->mutex);
                    exit(0);
                }

                sprintf(childX, "%d", GetRandRangeInt(0, kMapSizeX));
                sprintf(childY, "%d", GetRandRangeInt(0, kMapSizeY));
                sprintf(childTypeAnimal, "%d", memory->db_animals[index].type);
                char arg[]  = {childX, childY, childTypeAnimal};
                execv("animal", arg);
                pthread_mutex_unlock(&memory->mutex);
            }
            // Ест
            else if ((memory->db_animals[index].type + 1 ) % 3 == memory->db_animals[memory->map[x][y]].type){

                // Смотрим продолжительность жизни 
                if (memory->db_animals[index].life_time == 0){
                    pthread_mutex_unlock(&memory->mutex);
                    exit(0);
                }

                memory->db_animals[memory->map[x][y]].life_time = 0;

                memory->map[memory->db_animals[index].coord.x][memory->db_animals[index].coord.y] = 17;

                memory->db_animals[index].coord.x = x;
                memory->db_animals[index].coord.y = y;
                memory->db_animals[index].startvation_time = kStarvationTime;
                
                memory->map[x][y] = index;

                PrintMap(memory);

                pthread_mutex_unlock(&memory->mutex);
            }
            // Его едят
            else{
                // Смотрим продолжительность жизни 
                if (memory->db_animals[index].life_time == 0){
                    pthread_mutex_unlock(&memory->mutex);
                    exit(0);
                }

                memory->map[memory->db_animals[index].coord.x][memory->db_animals[index].coord.y] = 17;

                PrintMap(memory);

                pthread_mutex_unlock(&memory->mutex);
                exit(0);
            }
        }
        else
        {
            // Смотрим продолжительность жизни 
            if (memory->db_animals[index].life_time == 0){
                pthread_mutex_unlock(&memory->mutex);
                exit(0);
            }

            // Обнуление прошлой ячейки
            memory->map[memory->db_animals[index].coord.x][memory->db_animals[index].coord.y] = 17;
            
            // Переход
            memory->db_animals[index].coord.x = x;
            memory->db_animals[index].coord.y = y;
            memory->db_animals[index].life_time -= 1;
            memory->db_animals[index].startvation_time -= 1;   
            memory->map[x][y] = index;

            PrintMap(memory);
            pthread_mutex_unlock(&memory->mutex);
        }

        // Смотрим продолжительность жизни 
        if (memory->db_animals[index].life_time == 0){
            exit(0);
        }
        sleep(2);
    }
}


