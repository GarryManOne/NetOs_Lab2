#ifndef LAB2_H_INCLUDED
#define LAB2_H_INCLUDED

#include <stdio.h>
#include <pthread.h>

#define SHARED_MEMORY_OBJECT_NAME "shr_memory"


// ************************* Константы ************************************
// Продолжительность жизни (измеряется в количествах сделанных переходов)
const int kLifeTime = 15;    

// Продолжительность голодания (измеряется в количествах сделанных переходов)
const int kStarvationTime = 10;  

// Размеры карты 
const int kMapSizeX = 4;
const int kMapSizeY = 4;
// ************************************************************************


// **************************** Описание объектов *************************
// Виды животных
typedef enum {ANIMAL_1, ANIMAL_2, ANIMAL_3, NONE} TypeAnimal;

// Передвижение
typedef enum {RIGHT, LEFT, UP, DOWN} Direction;

// Координаты
typedef struct {
    int x;
    int y;
} Coordinate;

// Атрибуты животного
typedef struct {
    Coordinate coord;       // Координаты
    TypeAnimal type;        // Тип животного
    int startvation_time;   // Продолжительность голодания
    int life_time;          // Продолжительность жизни
} AnimalAttributes;

// Shared Memory
typedef struct 
{
    int file_map;
    pthread_mutex_t mutex;              // Мьютекс
    int map[4][4];                      // Карта
    AnimalAttributes db_animals[16];    // База данных о животных

} shr_mem;
// ************************************************************************

// *************************** Прототипы функций **************************
// Генерация псевдослучайных чисел на определнном промежутке
int GetRandRangeInt(int min, int max);

// Вывод карты в консоль
void PrintMap(void);
// ************************************************************************

#endif /* LAB2_H_INCLUDED */