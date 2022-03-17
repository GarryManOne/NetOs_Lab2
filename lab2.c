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


// // Функция, вызываемая при завершении процесса
// void ExitMessage(void)
// {
//     fprint("Процесс %d завершил работу.\n", getpid());
// }

// Генерация псевдослучайных чисел на определнном промежутке
int GetRandRangeInt(int min, int max){
    return rand() % (max - min) + min;
}

// // Функция работающая в отдельном потоке
void Animal(int atr){

    int index = atr;
    int n = 0;

    printf("%u-%d\n", getpid(),index);

    printf("%x\n", memory);

    while (1){
        // Смотрим продолжительность жизни 
        if (memory->db_animals[index].life_time == 0 || memory->db_animals[index].startvation_time == 0){
            memory->map[memory->db_animals[index].coord.x][memory->db_animals[index].coord.y] = 17;
            exit(0);
        }

        execv()
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

        printf("%u шаг=%d \n", getpid(), n++);

        // Проверка какое животное находится в этой ячейке
        if(memory->map[x][y] != 17){
            
            // Спаривание :)
            if (memory->db_animals[index].type == memory->db_animals[memory->map[x][y]].type){
                
                // Смотрим продолжительность жизни 
                if (memory->db_animals[index].life_time == 0){
                    pthread_mutex_unlock(&memory->mutex);
                    exit(0);
                }
                AddDataBase(GetRandRangeInt(0, kMapSizeX), GetRandRangeInt(0, kMapSizeY), memory->db_animals[index].type);
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

                PrintMap();

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

                PrintMap();

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

            PrintMap();
            pthread_mutex_unlock(&memory->mutex);
        }

        // Смотрим продолжительность жизни 
        if (memory->db_animals[index].life_time == 0){
            exit(0);
        }
        sleep(2);
    }
}

// Создание процесса
void AddDataBase(int row, int column, TypeAnimal type){
    
    for(int i = 0; i < kMapSizeX*kMapSizeY; i++){
        if (memory->db_animals[i].type == NONE){

            memory->db_animals[i].type = type;              
            memory->db_animals[i].coord.x = row;
            memory->db_animals[i].coord.y = column;
            memory->db_animals[i].life_time = kLifeTime;
            memory->db_animals[i].startvation_time = kStarvationTime;

            memory->map[row][column] = i;

            if (fork() == 0){
                
            }
            else{
                break;
            }
        }
    }

    // TODO: выйти если, все занято
}

// Вывод карты в консоль
void PrintMap(void){
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

// // Открытие файла для записи
// FILE* OpenFile(char* fileName){
//     FILE* file;
//     if ((file = fopen(fileName, "w")) == NULL){
//         printf("Не удалось открыть файл");
//         getchar();
//     }

//     return file;
// }                               

// Главная функция
int main(int argc, char *argv[]){

    srand(time(NULL));
    setbuf(stdout, 0);

    // int map_desc = open("map.txt", O_WRONLY | O_TRUNC | O_CREAT, 0777);

    int shm = 0;
    
    if( (shm = shm_open (SHARED_MEMORY_OBJECT_NAME, O_CREAT|O_RDWR, 0777)) == -1){
        perror("shm_open");
        return -1;
    }

    ftruncate(shm, sizeof(shr_mem));

    // Создаем отображение
    memory = mmap(NULL, sizeof(shr_mem), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    if (memory == (shr_mem*)-1){
        perror("mmap");
        return 1;
    }

    printf("%x\n", memory);
    memory->file_map = map_desc;

    // Обнуление карты 
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            memory->map[i][j] = 17;
        }
    }

    for (int i = 0; i < kMapSizeX*kMapSizeY; i++){
        memory->db_animals[i].type = NONE;
    }

    //  Создание и инициализация мьютексов
    pthread_mutex_init(&memory->mutex, NULL);    

    // Блокировка
    pthread_mutex_lock(&memory->mutex);

    // Создание процессов
    AddDataBase(0, 0, ANIMAL_1);
    AddDataBase(1, 3, ANIMAL_2);
    AddDataBase(3, 2, ANIMAL_3);
    AddDataBase(4, 0, ANIMAL_2);

    // Разблокировка
    pthread_mutex_unlock(&memory->mutex);

    // Ожидание завершения дочерних процессов
    // waitpid(-1, 0, 0);   
    sleep(30);

    // Освобождение памяти
    pthread_mutex_destroy(&memory->mutex);

    // Удаление разделяемой памяти
    shm_unlink(SHARED_MEMORY_OBJECT_NAME);

    printf("Все четко!\n");

    return 0;
}