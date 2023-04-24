#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "IsraeliQueue.h"
#include <string.h>


#define ID_LENGTH 10


//=================================================================
/*typedefs & struct declaration*/
//=================================================================


typedef struct Course_t{
    int m_number, m_size;
}*Course;

typedef struct Student_t{
    char m_studentID[ID_LENGTH];
    char *m_name;
    char *m_friendsList[ID_LENGTH];
    char *m_enemiesList[ID_LENGTH];
}*Student;

typedef struct EnrollmentSystem_t{

    Student *m_studentsList, *m_hackersList;
    Course *m_coursesList;
    int m_studentsNum, m_coursesNum, m_hackersNum;
    int **m_queues;



} *EnrollmentSystem;

//=================================================================================
//Inner Functions Declerations:
//=================================================================================

long int fileLength(FILE* file);


//==================================================================================
//LIBRARY FUNCTIONS: lord help
//==================================================================================

/**Creates a new Enrollment system based on the files provided, files should be
 * according to to following structure:
 * @param students: <Student ID> <Total Credits> <GPA> <Name> <Surname> <City> <Department>\n
 * @param courses: <Course Number> <size>\n
 * @param hackers: <Student ID>\n <Course Number>\n <Student ID>\n (friends) <Student ID>\n (rivals)
 * 
 * ALL MALLOCS SHOULD BE FREED IN DESTROY FUCTION??
 * returns a pointer
 * */
EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){

    
    if(!students||!courses||!hackers){
        return ;//BAD PARAM
    }

    EnrollmentSystem newSys=(EnrollmentSystem)malloc(sizeof(EnrollmentSystem));





    return newSys;
}

/**Function Description:
 * 
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues);

/**Function Description:
 * 
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out);


//=========================================================================
//Inner Functions
//=========================================================================


//returns file length and doesnt change the file ptr
long int fileLength(FILE* file){
    fseek(file,0,SEEK_END);
    long int length=ftell(file);
    fseek(file, 0, SEEK_SET);
    return length;

}

void examinFileLinesAndSize(FILE* file, int *lines, int *maxWordLength){
    if(!file){
        return; //BAD PARAM
    }
    int lineCouner=0,maxWord=0,tempMaxWord=1;
    char tempC;
    while ((tempC=fgetc(file))!=EOF){
        switch (tempC)
        {
        case ' ':
            if (maxWord<tempMaxWord){
                maxWord=tempMaxWord;
            }
            tempMaxWord=0;
            break;
        case '\n':
            lineCouner++;
            if (maxWord<tempMaxWord){
                maxWord=tempMaxWord;
            }
            tempMaxWord=0;
            break;
        default:
            tempMaxWord++;
            break;
        }
    }
    *lines=lineCouner;
    *maxWordLength=maxWord;

    
    
}









//=================================================================================
//FRIENDSHIP FUNCTIONS: they really explain themselves...
//==================================================================================


//TODO: rewrite this whole thing checks if 
int friendshipByHackerFile(Student stu, Student hacker);


//func desc: ill let you know when i find out
int friendshipByASCII(Student stu, Student hacker);


//func desc: ill let you know when i find out
int friendshipByIDQuetient(Student stu, Student hacker);


