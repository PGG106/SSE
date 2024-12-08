#pragma once

#include "shims.h"

bool next_token(const char* str, int* index, char* token);

//typedef struct {
//    size_t* data;       // Pointer to the array
//    size_t size;     // Number of elements currently in the array
//    size_t capacity; // Total capacity of the array
//} Vector_size_t;
//
//static void init_size_t(Vector_size_t* arr, size_t initialCapacity) {
//    arr->data = (size_t*)malloc(initialCapacity * sizeof(size_t));
//    arr->size = 0;
//    arr->capacity = initialCapacity;
//}
//
//static void push_back_size_t(Vector_size_t* arr, size_t value) {
//    if (arr->size == arr->capacity) {
//        if(arr->capacity == 0) {
//            arr->capacity = 2;
//        }
//
//        arr->capacity *= 2;
//        arr->data = (size_t*)realloc(arr->data, arr->capacity * sizeof(size_t));
//        if (!arr->data) {
//            perror("Failed to resize array");
//            exit(EXIT_FAILURE);
//        }
//    }
//    arr->data[arr->size++] = value;
//}
//
//static size_t clear_size_t(Vector_size_t* arr) {
//    arr->size = 0;
//}
//
//static void free_size_t(Vector_size_t* arr) {
//    free(arr->data);
//    arr->data = NULL;
//    arr->size = 0;
//    arr->capacity = 0;
//}