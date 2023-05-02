#include <stdio.h>
//#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "IsraeliQueue.h"
#include <string.h>
#include "FileHelper.h"


#define ID_LENGTH 10
#define STUDENT_TXT_ARG 7
#define ID_INDEX 0
#define F_NAME_INDEX 3
#define L_NAME_INDEX 4
#define RIVALRY_THR (-20)
#define FRIENDSHIP_THR 20
#define AMOUNT_OF_FF 3
//=================================================================
/*typedefs & struct declaration*/
//=================================================================


typedef struct Course_t{
    int m_number, m_size;
    IsraeliQueue m_queue;
}*Course;

typedef struct Student_t{
    int m_studentID;
    char *m_name;
    int *m_friendsList, *m_enemiesList;
} *Student;

typedef struct Hacker_t{
    int  *m_preferredCourses;
    Student m_studentCard;

} *Hacker;

typedef struct EnrollmentSystem_t{
    Student *m_studentsList;
    Hacker  *m_hackersList;
    Course *m_coursesList;
    int m_studentsNum, m_coursesNum, m_hackersNum;
    //int **m_queues;
    FriendshipFunction *m_functionArray;
} *EnrollmentSystem;

//=================================================================================
//Inner Functions Declarations:
//=================================================================================

Student *createStudentListFromFile(FILE* students, int *length);
Course *createCourseListFromFile(FILE* courses, int *length);
Hacker *createHackersListFromFile(FILE* hackers, int *length, Student *studentList,int maxArrayNum);
Student findStudentByID(Student *studentList, int ID);




//==================================================================================
//LIBRARY FUNCTIONS: lord help
//==================================================================================


void destroyEnrollment(EnrollmentSystem sys){
    if(!sys){
        return;//is this an error?
    }
    if (sys->m_studentsList){
        for (int i=0;i<sys->m_studentsNum;i++){
            free(sys->m_studentsList[i]->m_name);
            free(sys->m_studentsList[i]->m_friendsList);
            free(sys->m_studentsList[i]->m_enemiesList);
        }
    }
    free(sys->m_studentsList);
    if(sys->m_hackersList){
        for(int i=0;i<sys->m_hackersNum;i++){
            free(sys->m_hackersList[i]->m_preferredCourses);
        }
    }
    free(sys->m_hackersList);
    if(sys->m_coursesList){
        for (int i=0; i<sys->m_coursesNum; i++){
            //TODO put destroy queue
        }
    }
    free(sys->m_coursesList);
    free(sys->m_functionArray);
    free(sys);
}



/**Creates a new Enrollment system based on the files provided, files should be
 * according to to following structure:
 * @param students: <Student ID> <Total Credits> <GPA> <Name> <Surname> <City> <Department>\n
 * @param courses: <Course Number> <size>\n
 * @param hackers: <Student ID>\n <Course Number>\n <Student ID>\n (friends) <Student ID>\n (rivals)
 * 
 * ALL MALLOC S SHOULD BE FREED IN DESTROY FUNCTION??
 * returns a pointer
 * */

EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){
    if(!students||!courses||!hackers){
        return NULL;//BAD PARAM
    }
    EnrollmentSystem newSys=(EnrollmentSystem)malloc(sizeof(EnrollmentSystem));
    if (!newSys){
        return NULL;//MALLOC FAIL
    }
    newSys->m_studentsList= createStudentListFromFile(students,&(newSys->m_studentsNum));
    newSys->m_coursesList= createCourseListFromFile(courses,&(newSys->m_coursesNum));
    int maxArr=max(newSys->m_coursesNum,newSys->m_studentsNum);
    newSys->m_hackersList= createHackersListFromFile(hackers,&(newSys->m_hackersNum),newSys->m_studentsList, maxArr);
    newSys->m_functionArray= malloc(sizeof (FriendshipFunction)*AMOUNT_OF_FF);

    if (!newSys->m_functionArray||!newSys->m_hackersList||!newSys->m_coursesList||!newSys->m_studentsList){
        destroyEnrollment(newSys);
        return NULL;//MALLOC FAIL
    }
    // TODO: newSys->m_functionArray={ &friendshipByIDDiff, &friendshipByASCII, &friendshipByHackerFile};
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

/**Function Description:
 *
 * */
int isTheSameStudent(void* stuA, void* stuB){
    Student student1=(Student)stuA, student2=(Student)stuB;
    if(student1->m_studentID==student2->m_studentID){
        return 1;
    }
    return 0;
}


//=========================================================================
//Inner Functions
//=========================================================================
//malloc notes: all objects and strings in object should be freed in destroy function
Student *createStudentListFromFile(FILE* students, int *length){
    assert(students!=NULL);
    int maxNameSize= getMaxWordInFile(students), lines= getLineNumInFile(students);
    char *tempWordStr=(char*) malloc(sizeof (char)*maxNameSize);
    char *studentName=(char*) malloc(sizeof (char )*(maxNameSize*2+2));
    Student *studentList=(Student*)malloc((sizeof(Student))*(lines+1));
    if (!studentList||!tempWordStr||!studentName){
        return NULL;//MALLOC FAIL
    }
    for(int k=0;k<lines;k++){

        studentList[k]=(Student)malloc(sizeof(Student));
        if (studentList[k]){
            return NULL;//MALLOC FAIL
        }
        for (int i=0;i<STUDENT_TXT_ARG;i++){
            switch (i) {
                case ID_INDEX:
                    putNextWordToString(students, tempWordStr);
                    studentList[i]->m_studentID=strtol(tempWordStr,&tempWordStr, 10);
                    break;
                case F_NAME_INDEX:
                    putNextWordToString(students, tempWordStr);
                    strcpy(studentName,tempWordStr);
                    break;
                case L_NAME_INDEX:
                    putNextWordToString(students, tempWordStr);
                    strcat(studentName, " ");
                    strcat(studentName, tempWordStr);
                    break;
                default:
                    putNextWordToString(students,tempWordStr);
                    break;
            }
        }
        studentList[k]->m_name= (char*)malloc(sizeof(char)*(strlen(studentName)));
        if(!studentList[k]->m_name){
            return NULL;// MALLOC FAIL
        }
        strcpy(studentList[k]->m_name,studentName);
        strcat(studentList[k]->m_name," ");
        studentList[k]->m_enemiesList=NULL;
        studentList[k]->m_friendsList=NULL;
    }
    free(tempWordStr);
    free(studentName);
    length[0]=lines;
    studentList[lines]=NULL;
    return studentList;
}


Course *createCourseListFromFile(FILE* courses, int *length){
    assert(courses!=NULL);
    int maxWord= getMaxWordInFile(courses),lines= getLineNumInFile(courses);
    Course *courseList=(Course*)malloc(sizeof(Course)*(lines+1));
    char *tempStr=(char*) malloc(sizeof (char)*(maxWord*2+2));
    if (!courseList||!tempStr){
        return NULL;//MALLOC FAIL
    }
    for (int i=0;i<lines;i++){
        courseList[i]=(Course)malloc(sizeof(Course));
        if(!courseList[i]){
            return NULL;//MALLOC FAIL
        }
        putNextWordToString(courses,tempStr);
        courseList[i]->m_number= strtol(tempStr,&tempStr,10);
        putNextWordToString(courses,tempStr);
        courseList[i]->m_size= strtol(tempStr,&tempStr,10);
        courseList[i]->m_queue= IsraeliQueueCreate(NULL,isTheSameStudent,FRIENDSHIP_THR,RIVALRY_THR);
        if (!courseList[i]->m_queue){
            return NULL;//CREATE FAIL
        }
    }
    courseList[lines]=NULL;
    free(tempStr);
    length[0]=lines;
    return courseList;
}


Hacker *createHackersListFromFile(FILE* hackers, int *length,Student *studentList, int maxArrayNum){
    assert(hackers!=NULL);
    int maxLine= getMaxLineInFile(hackers) ,linesNum= getLineNumInFile(hackers);
    Hacker *hackerList=(Hacker*) malloc(sizeof(Hacker)*((linesNum/4)+1));
    char *tempStr=(char*) malloc(sizeof(char)*maxLine);
    if (!tempStr||!hackerList){
        return NULL;//MALLOC FAIL
    }
    int *tempArray=(int*)malloc(sizeof(int)*maxArrayNum);
    if(!tempArray){
        return NULL;//MALLOC FAIL
    }
    for (int i=0;i<linesNum/4;i++){
        putNextWordToString(hackers,tempStr);
        hackerList[i]=(Hacker)malloc(sizeof(Hacker));
        if(!hackerList[i]){
            return NULL;//MALLOC FAIL;
        }
        hackerList[i]->m_studentCard= findStudentByID(studentList, strtol(tempStr,&tempStr,10));
        if(!hackerList[i]->m_studentCard){
            return NULL;//couldn't find student;
        }
        //parse line to courses:
        putNextLineToString(hackers,tempStr);
        hackerList[i]->m_preferredCourses= fillIntArrayFromStr(tempArray,tempStr);
        if(!hackerList[i]->m_preferredCourses){
            return NULL; //MALLOC FAIL
        }
        resetIntArray(tempArray,maxArrayNum);

        //parse line to friends:

        putNextLineToString(hackers,tempStr);

        hackerList[i]->m_studentCard->m_friendsList=fillIntArrayFromStr(tempArray,tempStr);
        if(!hackerList[i]->m_studentCard->m_friendsList){
            return NULL; //MALLOC FAIL
        }
        resetIntArray(tempArray,maxArrayNum);

        //parse line to enemies:
        putNextLineToString(hackers,tempStr);
        hackerList[i]->m_studentCard->m_enemiesList=fillIntArrayFromStr(tempArray,tempStr);
        if(!hackerList[i]->m_studentCard->m_enemiesList){
            return NULL; //MALLOC FAIL
        }
        resetIntArray(tempArray,maxArrayNum);
    }
    hackerList[linesNum/4]=NULL;
    length[0]=linesNum/4;
    return hackerList;
}

//=========================================================
//inner inner functions
//=========================================================

//
//finds student from list and returns struct pointer
// if not found in list returns null
Student findStudentByID(Student *studentList, int ID){
    if(!studentList){
        return NULL;//BAD PARAM
    }
    int i=0;
    while(studentList[i]!=NULL){
        if(studentList[i]->m_studentID==ID){
            return studentList[i];
        }
        i++;
    }
    return NULL;
}

void swap(Student *studentA,Student *studentB){
    Student temp1=*studentA;
    *studentA=*studentB;
    *studentB=temp1;
}


//=================================================================================
//FRIENDSHIP FUNCTIONS: they really explain themselves...
//==================================================================================


//TODO:  checks if works
int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB){
    if(!ptrStudentA||!ptrStudentB){
        return -1;//BAD PARAM
    }
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;

    int i=0;
    if (studentA->m_friendsList==NULL){
        swap(&studentA,&studentB);
    }

    while(studentA->m_friendsList[i]!=0){
        if (studentA->m_friendsList[i++]==studentB->m_studentID){
            free(studentA);
            free(studentB);
            return 20;
        }
    }
    i=0;
    while(studentA->m_enemiesList[i]!=0){
        if (studentA->m_enemiesList[i++]==studentB->m_studentID){
            free(studentA);
            free(studentB);
            return -20;
        }
    }
    return 0;
}



int friendshipByASCII(void* ptrStudentA, void* ptrStudentB){
    if(!ptrStudentA||!ptrStudentB){
        return 0;//BAD PARAM ??? TODO check return value
    }
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int stuANameLen=(int)strlen(studentA->m_name), stuBNameLen=(int)strlen(studentB->m_name), sum=0;
    if (stuANameLen<stuBNameLen){
        swap(&studentA,&studentB);
    }
    for (int i=0;i<stuBNameLen;i++){
        sum+= abs((int)(studentA->m_name[i]-studentB->m_name[i]));
    }
    for (int i=stuBNameLen;i<stuANameLen;i++)
        sum+=(int)studentA->m_name[i];

    return sum;
}



int friendshipByIDDiff(void* ptrStudentA, void* ptrStudentB){
    if(!ptrStudentA||!ptrStudentB){
        return -1;//BAD PARAM
    }
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int sum=0, pow=0;
    for (int i=0; i<ID_LENGTH;i++){
        sum+=abs((studentA->m_studentID%pow-studentA->m_studentID/pow)-(studentB->m_studentID%pow-studentB->m_studentID/pow));
        pow*=10;
    }
    return sum;
}


int putNextWordToString(FILE* file, char* str){
    assert(file!=NULL&&str!=NULL);
    int i=0;
    char temp=(char)fgetc(file);
    while (temp!=' '){
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