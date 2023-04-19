#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "IsraeliQueue.h"
#include <string.h>


#define ID_LENGTH 9
//=================================================================
/*typedefs & struct declaration*/
//=================================================================

typedef struct EnrollmentSystem_t{
    Student m_studentsHeadNode;
    Course m_coursesHeadNode;
    int m_studentsNum, m_coursesNum;//do we need this? i really dont know i miss arrays what is this node shit

}*EnrollmentSystem;

/*typedef struct Hacker_t{
    char m_studentID[10];
    Course *m_desiredCourses;
    Student *m_friends, *m_enemies;

    

}*Hacker;
*/

typedef struct Student_t{

    char m_studentID[ID_LENGTH+1];
    int m_totalCredits; //positive
    double m_GPA;
    char *m_firstName, *m_lastName, *m_city, *m_department;
    Course *m_desiredCourses;
    Student *m_friends, *m_enemies;

}*Student;

typedef struct Course_t{
    int m_number, m_size;
}*Course;



//==================================================================================
//LIBRARY FUNCTIONS: lord help
//==================================================================================

/**Creates a new Enrollment system based on the files provided, files should be
 * according to to following structure:
 * @param students: <Student ID> <Total Credits> <GPA> <Name> <Surname> <City> <Department>\n
 * @param courses: <Course Number> <size>\n
 * @param hackers: <Student ID>\n <Course Number>\n <Student ID>\n (friends) <Student ID>\n (rivals)
 * 
 * 
 * returns a pointer
 * */
EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){

    
}

/**Function Description:
 * 
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues);


/**Function Description:
 * 
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out);



//=================================================================================
//FRIENDSHIP FUNCTIONS: they really explain themselves...
//==================================================================================


//checks if 
int friendshipByHackerFile(Student stu, Student hacker){

    if (stu==NULL||hacker==NULL){
        return -1;// BAD PARAM
    }
    int i=0;
    while(stu->m_friends[i]!=NULL){
        if(stu->m_friends[i]->m_studentID==hacker->m_studentID){
            return 20;
        }
    } 
    i=0;
    while(stu->m_enemies[i]!=NULL){
        if(stu->m_enemies[i]->m_studentID==hacker->m_studentID){
            return 20;
        }
    } 
    return 0;

}


//func desc: ill let you know when i find out
int friendshipByASCII(Student stu, Student hacker){

    if (stu==NULL||hacker==NULL){
        return -1;// BAD PARAM
    }

    int ASCIISumStudent=0,ASCIISumHacker=0, i=0;
    while(stu->m_firstName[i]!=NULL){
        ASCIISumStudent+=(int)(stu->m_firstName[i++]);
    }
    i=0;
    while(stu->m_lastName[i]!=NULL){
        ASCIISumStudent+=(int)(stu->m_lastName[i++]);
    }
    i=0;
    while(hacker->m_firstName[i]!=NULL){
        ASCIISumHacker+=(int)(hacker->m_firstName[i++]);
    }
    i=0;
    while(hacker->m_lastName[i]!=NULL){
        ASCIISumHacker+=(int)(hacker->m_lastName[i++]);
    }
    return ASCIISumHacker-ASCIISumStudent;//??????????
}


//func desc: ill let you know when i find out
int friendshipByIDQuetient(Student stu, Student hacker){

    if (stu==NULL||hacker==NULL){
        return -1;// BAD PARAM
    }

    int stuIDSum=0, hackerIDSum=0, i=0;
    for (;i<ID_LENGTH;i++){
        stuIDSum+=stu->m_studentID[i];
        hackerIDSum+=hacker->m_studentID[i];
    }
    return stuIDSum-hackerIDSum;//????????
}


