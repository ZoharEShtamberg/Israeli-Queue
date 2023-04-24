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
    int m_studentID;
    char *m_name;
    Course *m_preferredCourses;
    int *m_friendsList, *m_enemies;
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
void examineFileLinesAndSize(FILE* file, int *lines, int *maxWordLength);
Student *createStudentListFromFile(FILE* students, int *length);
Student findByID(EnrollmentSystem sys, char* ID);


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
        return NULL;//BAD PARAM
    }

    EnrollmentSystem newSys=(EnrollmentSystem)malloc(sizeof(EnrollmentSystem));
    int studentsNum;
    newSys->m_studentsList= createStudentListFromFile(students,&studentsNum);
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

Student *createStudentListFromFile(FILE* students, int *length){
    int maxNameSize;
    examineFileLinesAndSize(students,length,&maxNameSize);
    int ID;
    char name[maxNameSize+1], lastName[maxNameSize+1], trashStr[maxNameSize+1];
    char temp='a';

    Student *studentList=(Student*)malloc(sizeof(Student)*(*length));


    do {
            for(int k=0;k<*length;k++){
                for (int i=0;i<7;i++){
                        switch (i){
                            case 0:
                                fscanf(students,"%d",ID);
                                break;
                            case 3:
                                fgets(name,maxNameSize,students);
                                break;
                            case 4:
                                fgets(lastName,maxNameSize,students);
                                break;
                            default:
                                fgets(trashStr,maxNameSize,students);
                        }
                        studentList[k]->m_studentID=ID;
                        studentList[k]->m_name= (char*)malloc(sizeof(char)*(strlen(name)+strlen(lastName)+2));
                        if(!studentList[k]->m_name){
                            return NULL;// MALLOC FAIL
                        }
                        strcpy(studentList[k]->m_name,name+' ');
                        strcat(studentList[k]->m_name,lastName);
                }
            }

    }
    while (fgetc(students)!=EOF);
    return studentList;
}


//returns file length and doesn't change the file ptr
long int fileLength(FILE* file){
    fseek(file,0,SEEK_END);
    long int length=ftell(file);
    fseek(file, 0, SEEK_SET);
    return length;
}

void examineFileLinesAndSize(FILE* file, int *lines, int *maxWordLength){
    if(!file){
        return; //BAD PARAM
    }
    int lineCounter=0,maxWord=0,tempMaxWord=1;
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
            lineCounter++;
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
    *lines=lineCounter;
    *maxWordLength=maxWord;
    fseek(file, 0, SEEK_SET);

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


