//
// Created by Daniella on 02/05/2023.
//

#include "FileHelper.h"
#include "string.h"
#include <assert.h>


/**
 *returns the size of the biggest line in file
 *resets FAP to set
 */
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


/**
 *returns the size of the biggest word in file
 *resets FAP to set
 */
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


/**
 *returns the size of the amount of lines in file
 *resets FAP to set
 */
int getLineNumInFile(FILE* file){
    if(!file){
        return -1; //BAD PARAM
    }
    int lineCounter=0;
    char tempC;
    while ((tempC=(char)fgetc(file))!=EOF){
        if(tempC=='\n') {
            lineCounter++;
        }
    }
    fseek(file, 0, SEEK_SET);
    return lineCounter;
}


/**creates new dynamically allocated array of int based on a string of numbers
 *str- string of numbers divided by " "
 * assumes line has at least one word
 */
int *createIntArrayFromStr(char* str, int* length){
    assert(str[0]);
    int size= 1,strIndex=0;
    while(str[strIndex]&&str[strIndex]!='\0'){
        if(str[strIndex]==' '){
            size++;
        }
        strIndex++;
    }
    *length=size;
    int *newArr= (int*)malloc(sizeof (int)*(size));
    if (!newArr){
        return NULL;
    }
    char *token= strtok(str, " ");

    for (int i = 0; i < size; i++) {
        newArr[i]=atoi(token);
        token= strtok(NULL, " ");
    }



    return newArr;
}

///*
// * assumes str has enough space for next line
// * puts next line in file into provided string. in case of error returns false
// * returns amount of chars copied
//*/
int putLineFromFileInString(char* destStr, FILE* file ){
    assert(file&&destStr);
    int i=0;
    char currChar=(char)fgetc(file);
    while (currChar&&currChar!='\n'&&currChar!='\r'){
        destStr[i++]=currChar;
        currChar=(char)fgetc(file);
    }
    if (i!=0){
        destStr[i]='\0';
    }
    if (currChar == '\r') {
        currChar = (char)fgetc(file);
        if (currChar != '\n') {
            fseek(file, -1, SEEK_CUR);  //Rewind if not  newline
        }
    }
    return i;
}

/**
 * returns distance to next word in srcStr to destStr, returns -1 if
 * USER responsible to make sure there is enough space!
 * @ returns amount of characters in a word
 *
 */
int getNextWordLength(const char* srcStr){
    int i=0;
    while(srcStr[i]){
        if (srcStr[i]==' '||srcStr[i]=='\n'||srcStr[i]=='\0'){
            return i-1;
        }
        i++;
    }
    return i;
}