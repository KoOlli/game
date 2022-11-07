#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

// Размеры поля
#define upperBorder 0
#define lowerBorder 25
#define leftBorder  0
#define rightBorder 80

#define MENU_OFFSET 30

#define VARIANT_A "script1.txt"
#define VARIANT_B "script2.txt"
#define VARIANT_C "script3.txt"
#define VARIANT_D "script4.txt"
#define VARIANT_E "script5.txt"

#define ESC 'q'

#define MIN_SPEED 100000
#define MAX_SPEED 300000


void game(int, int*);
int** initialization();
int countLiveCells(int**, int, int);
void generationCalculating(int**, int**);
void swapGenerations(int***, int***);
void printGeneration(int**, int);
void clearLastFrame();
void initncurses();
void endncurses();
void memFree(int***, int***);
void drawStartMenu(int);
void menu();
int** initializationPrefab(int);
FILE* chooseFile(int);

int main(void) {
    initncurses();
    menu();
    endncurses();
    return 0;
}
// Запускает меню
void menu() {
    int gamespeed = 300000;
    int running = 1;
    while (running) {
        drawStartMenu(gamespeed);
        char controlKey = getch();
        if (controlKey == '-' && gamespeed < MAX_SPEED)
            gamespeed += 100000;
        if (controlKey == '=' && gamespeed > MIN_SPEED)
            gamespeed -= 100000;
        if (controlKey >= '1' && controlKey <= '6')
            game((int)controlKey - '0', &gamespeed);
        if (controlKey == ESC) {
            running = 0;
        }
        usleep(10000);
    }
}
// Основная логика игры
void game(int variant, int* gamespeed) {
    srand(time(NULL));
    int** firstGeneration;
    if (variant == 6)
        firstGeneration = initialization();
    else
        firstGeneration = initializationPrefab(variant);

    int** secondGeneration = initialization();

    int running = 1;
    while (running) {
        char controlKey = getch();
        if (controlKey == '-' && *gamespeed < MAX_SPEED)
            *gamespeed += 100000;
        if (controlKey == '=' && *gamespeed > MIN_SPEED)
            *gamespeed -= 100000;
        if (controlKey == ESC) {
            running = 0;
        }
        clearLastFrame();
        printGeneration(firstGeneration, *gamespeed);
        generationCalculating(firstGeneration, secondGeneration);
        swapGenerations(&firstGeneration, &secondGeneration);

        usleep(*gamespeed);
    }
    memFree(&firstGeneration, &secondGeneration);
}
// Создание матрицы поколения и заполнение случайными элементами 0 --- 1
int** initialization() {
    int** matrix = (int**)malloc(lowerBorder * sizeof(int*));
    for (int i = 0; i < lowerBorder; i++) {
        matrix[i] = (int*)malloc(rightBorder * sizeof(int));
        for (int j = 0; j < rightBorder; j ++)
            matrix[i][j] = rand()%2;
    }
    return matrix;
}
// Вариант начальной инициализации матрицы определенным префабом из файла
int** initializationPrefab(int variant) {
    FILE *input = chooseFile(variant);
    int** matrix = (int**)malloc(lowerBorder * sizeof(int*));
    for (int i = 0; i < lowerBorder; i++) {
        matrix[i] = (int*)malloc(rightBorder * sizeof(int));
        for (int j = 0; j < rightBorder; j ++)
            fscanf(input, "%d", &matrix[i][j]);
    }
    fclose(input);
    return matrix;
}
// Выбор файла подгрузки стартового значения
FILE* chooseFile(int variant) {
    FILE* input;
    switch (variant) {
        case 1:
            input = fopen(VARIANT_A, "r");
            break;
        case 2:
            input = fopen(VARIANT_B, "r");
            break;
        case 3:
            input = fopen(VARIANT_C, "r");
            break;
        case 4:
            input = fopen(VARIANT_D, "r");
            break;
        case 5:
            input = fopen(VARIANT_E, "r");
            break;
    }
    return input;
}
// Расчет нового поколения
void generationCalculating(int** firstGeneration, int** secondGeneration) {
    int neighbours;
    for (int i = upperBorder; i < lowerBorder; i++) {
        for (int j = leftBorder; j < rightBorder; j++) {
            neighbours = countLiveCells(firstGeneration, i, j);
            if ((neighbours  < 2 || neighbours > 3) && firstGeneration[i][j] == 1)
                secondGeneration[i][j] = 0;
            else if ((neighbours == 2 || neighbours == 3) && firstGeneration[i][j] == 1)
                    secondGeneration[i][j] = 1;
            else if (neighbours == 3 && firstGeneration[i][j] == 0)
                    secondGeneration[i][j] = 1;
            else
                secondGeneration[i][j] = 0;
        }
    }
}
// Подсчет количества соседей
int countLiveCells(int** fieldMatrix, int line, int column) {
    int count = 0;
    for (int i = line - 1; i <= line + 1; i++)
        for (int j = column - 1; j <= column + 1; j++) {
            if ((i == line) && (j == column)) continue;
            else if (fieldMatrix[(i+lowerBorder)%lowerBorder][(j + rightBorder) % rightBorder])count++;
        }
    return count;
}
// Вывод поколения в терминал
void printGeneration(int** fieldMatrix, int gamespeed) {
    printw("\t\t\t\t\t\t\t<=============> GAME SPEED x %d.00 <=============>\n", 4 - gamespeed/100000);
    for (int i = upperBorder; i < lowerBorder; i++) {
        for (int j = leftBorder; j < rightBorder; j++)
            if (fieldMatrix[i][j] == 1) printw(" *");
            else
                printw("  ");
        printw("\n");
    }
    printw("\t\t\t\t\t\t\t<=================> Q FOR QUIT <=================>\n");
    refresh();
}
// Обмен адресов поколений
void swapGenerations(int*** first, int*** second) {
    int** buf = *first;
    *first = *second;
    *second = buf;
}
// Функция отрисовки меню
void drawStartMenu(int gamespeed) {
    clearLastFrame();
    for (int i = 0; i < lowerBorder; i++) {
       for (int j = 0; j < rightBorder; j++) {
            if (i == 0 || i == lowerBorder - 1) printw("-");
            else if (i == 1 && j == MENU_OFFSET + 2)printw("GAME SPEED x %d.00\n", 4 - gamespeed/100000);
            else if (i == 2 && j == MENU_OFFSET-10)printw("<==============> GAME MENU <==============>");
            else if (i == 4 && j == MENU_OFFSET)printw("PRESS 1 FOR VARIANT A");
            else if (i == 6 && j == MENU_OFFSET)printw("PRESS 2 FOR VARIANT B");
            else if (i == 8 && j == MENU_OFFSET)printw("PRESS 3 FOR VARIANT C");
            else if (i == 10 && j == MENU_OFFSET)printw("PRESS 4 FOR VARIANT D");
            else if (i == 12 && j == MENU_OFFSET)printw("PRESS 5 FOR VARIANT E");
            else if (i == 14 && j == MENU_OFFSET- 1)printw("PRESS 6 FOR HOLY RANDOM");
            else if (i == 16 && j == MENU_OFFSET + 3)printw("PRESS Q FOR EXIT");
            else if (i == 18 && j == MENU_OFFSET - 10)printw("<=========================================>");
            else
                printw(" ");
        }
        printw("\n");
    }
}
// Освобождение памяти
void memFree(int*** array1, int*** array2) {
    for (int i = 0; i < lowerBorder; i++) {
        free(*(*array1 + i));
        free(*(*array2 + i));
        *(*array1 + i) = NULL;
        *(*array1 + i) = NULL;
    }
    free(*array1);
    free(*array2);
    *array1 = NULL;
    *array2 = NULL;
}
// Очистка кадра терминала
void clearLastFrame() {
    clear();
}
// Инициализация ncurses
void initncurses() {
    initscr();
    raw();  // Обрабатывает ввод с клавиатуры
    nodelay(stdscr, 1);  // Терминал устанавливается в неблокируемый режим
    noecho();  // Скрывает символы в терминале
    curs_set(0);  // Скрывает курсор в терминале
}
// Завершение работы ncurses
void endncurses() {
    echo();
    endwin();
}
