//
// Created by Daniella on 02/05/2023.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifndef EX1_CLION_FILEHELPER_H
#define EX1_CLION_FILEHELPER_H

void examineFileLines(FILE* file, int *lines, int *maxLineLength);

long int fileLength(FILE* file);
void copyIntArray(const int src[],int dest[], int length);
int max(int a, int b);
int putNextWordToString(FILE* file, char* str);
int getMaxLineInFile(FILE* file);
int getMaxWordInFile(FILE* file);
int getLineNumInFile(FILE* file);
int putNextLineToString(FILE* file, char* str);
void resetIntArray(int *arr, int size);
int *fillIntArrayFromStr(int *arr, char *str);

#endif //EX1_CLION_FILEHELPER_H
