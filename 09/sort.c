#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 時間計測をする際には大きな数値にしてください。
#define SIZE 10

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void simple_sort(int* array) {
    for (int i = 0; i < SIZE - 1; i++) {
        for (int j = i + 1; j < SIZE; j++) {
            if (array[i] > array[j]) {
                swap(&array[i], &array[j]);
            }
        }
    }
}

void bubble_sort(int* array) {
    for (int i = 0; i < SIZE - 1; i++) {
        for (int j = SIZE - 1; j > i; j--) {
            if (array[j - 1] > array[j]) {
                swap(&array[j - 1], &array[j]);
            }
        }
    }
}

void selection_sort(int* array) {
    for (int i = 0; i < SIZE - 1; i++) {
        int min_pos = i;
        for (int j = i + 1; j < SIZE; j++) {
            if (array[j] < array[min_pos]) {
                min_pos = j;
            }
        }
        swap(&array[i], &array[min_pos]);
    }
}

void insertion_sort(int* array) {
    for (int i = 1; i < SIZE; i++) {
        for (int j = i - 1; j >= 0; j--) {
            if (array[j] <= array[j + 1]) break;
            swap(&array[j], &array[j + 1]);
        }
    }
}

void print(int* array) {
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void run_sort_algorithm(int* array, void (*sort_algorithm)(int*)) {
    double start_clock, end_clock;
    start_clock = (double)clock();
    sort_algorithm(array);
    end_clock = (double)clock();
    printf("Time  : %.8lf s\n", (end_clock - start_clock) / CLOCKS_PER_SEC);
}

void shuffle(int* array, int length) {
    int i = length;
    while (i > 1) {
        int j = rand() % i--;
        swap(&array[i], &array[j]);
    }
}

int main() {
    int keys[SIZE];
    for (int i = 0; i < SIZE; i++) {
        keys[i] = i;
    }
    shuffle(keys, SIZE);

    int* array = (int*)malloc(sizeof(int) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        array[i] = keys[i];
    }

    printf("Before: ");
    print(array);
    run_sort_algorithm(array, simple_sort);
    // run_sort_algorithm(array, bubble_sort);
    // run_sort_algorithm(array, insertion_sort);
    printf("After : ");
    print(array);

    return 0;
}

// 実行結果
// Before: 9 8 4 2 0 6 5 1 7 3
// Time  : 0.00000100 s
// After : 0 1 2 3 4 5 6 7 8 9
