//
// Created by Daniella on 02/05/2023.
//

#include "FileHelper.h"
#include "string.h"
#include <assert.h>



//returns the size of the biggest line in file
//resets FAP to set
int getMaxLineInFile(FILE* file){
    if(!file){
        return -1; //BAD PARAM
    }
    int  maxLine=0, tempMaxLine=0;
    char tempC;
    while ((tempC=(char)fgetc(file))!=EOF){
        if(tempC!='\n'){
            tempMaxLine++;
        }
        else{
            if (tempMaxLine>maxLine){
                maxLine=tempMaxLine;
            }
            tempMaxLine=0;
        }
    }
    if (tempMaxLine>maxLine){
        maxLine=tempMaxLine;
    }
    fseek(file, 0, SEEK_SET);
    return maxLine;

}


int getMaxWordInFile(FILE* file){
    if(!file){
        return -1; //BAD PARAM
    }
    int maxWord=0, tempMaxWord=1;
    char tempC;
    while ((tempC=(char)fgetc(file))!=EOF){
        if(tempC!='\n'&&tempC!=' '){
            tempMaxWord++;
        }
        else{
            if (tempMaxWord>maxWord){
                maxWord=tempMaxWord;
            }
            tempMaxWord=0;
        }
    }
    if (tempMaxWord>maxWord){
        maxWord=tempMaxWord;
    }
    fseek(file, 0, SEEK_SET);
    return maxWord;

}

int getLineNumInFile(FILE* file){
    if(!file){
        return -1; //BAD PARAM
    }
    int lineCounter=0;
    char tempC;
    while ((tempC=(char)fgetc(file))!=EOF){
        if(tempC=='\n'){
            lineCounter++;
        }
    }
    fseek(file, 0, SEEK_SET);
    return lineCounter;

}

//creates new dynamically allocated array of int based on a string of numbers
//str- string of numbers divided by " "
int *createIntArrayFromStr(char* str){
    assert(str[0]);
    int size= 1,strIndex=0;
    while(str[strIndex]!='\n'){
        if(str[strIndex]==' '){
            size++;
        }
        strIndex++;
    }
    int *newArr= (int*)malloc(sizeof (int)*(size));
    if (!newArr){
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        newArr[i]=atoi(strtok(str," "));
    }

    return newArr;
}

int max(int a, int b){
    if (a>b)
        return a;
    return b;
}

