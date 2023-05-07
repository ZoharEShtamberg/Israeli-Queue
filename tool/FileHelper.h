//
// Created by Daniella on 02/05/2023.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifndef EX1_CLION_FILEHELPER_H
#define EX1_CLION_FILEHELPER_H


int getMaxLineInFile(FILE* file);
int getMaxWordInFile(FILE* file);
int getLineNumInFile(FILE* file);
int *createIntArrayFromStr(char *str);
int putLineFromFileInString(char* str, FILE* file);

#endif //EX1_CLION_FILEHELPER_H
