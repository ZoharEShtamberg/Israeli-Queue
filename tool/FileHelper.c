//
// Created by Daniella on 02/05/2023.
//

#include "FileHelper.h"
#include <assert.h>


void copyIntArray(const int src[],int dest[], int length){
    for (int i=0;i<length; i++){
        dest[i]=src[i];
    }
}

//returns file length and doesn't change the file ptr
long int fileLength(FILE* file){
    fseek(file,0,SEEK_END);
    long int length=ftell(file);
    fseek(file, 0, SEEK_SET);
    return length;
}

//saves lines in file and maximum word length to ptr
//resets FAP to set
int getMaxLineInFile(FILE* file){
    if(!file){
        return -1; //BAD PARAM
    }
    int  maxLine=0, tempMaxLine=1;
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

//creates new dynamically allocated array of int based on a string
//@PARAM:str- string of numbers divided by " "
//arr: a temp array with the max size of the input
//the returned array is of size n+1 where n is the number of numbers and arr[n]=-1
int *fillIntArrayFromStr(int *arr, char* str){
    int i=0;
    while(str[0]!='\0'){
        arr[i++]= strtol(str,&str,10);
    }
    int *newArr= (int*)malloc(sizeof (int)*(i+1));
    if (!newArr){
        return NULL;
    }
    copyIntArray(arr,newArr,i);
    newArr[i]=-1;
    return newArr;
}
void resetIntArray(int *arr, int size){
    for(int i=0; i<size;i++){
        arr[i]=0;
    }
}
int max(int a, int b){
    if (a>b)
        return a;
    return b;
}



int putNextWordToString(FILE* file, char* str){
    assert(file!=NULL&&str!=NULL);
    int i=0;
    char temp=(char)fgetc(file);
    while (temp!=' '&&temp!='\n'){
        str[i++]=temp;
        temp=(char)fgetc(file);
    }
    str[i]='\0';
    return i+1;
}

int putNextLineToString(FILE* file, char* str){
    assert(file!=NULL&&str!=NULL);
    int i=0;
    char temp=(char)fgetc(file);
    while (temp!='\n'){
        str[i++]=temp;
        temp=(char)fgetc(file);
    }
    str[i]='\0';
    return i+1;
}