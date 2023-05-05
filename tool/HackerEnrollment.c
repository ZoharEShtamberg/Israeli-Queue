#include <stdio.h>
//#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "../IsraeliQueue.h"
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
Course findCourseByID(Course *courseList, int ID);
Student createStudentFromLine(char* str,int maxWord);




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
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues){
    if(!sys||!queues){
        return NULL;//BAD PARAM
    }
    char* tempStr=(char*)malloc(sizeof(char)* getMaxWordInFile(queues+1));
    if (!tempStr){
        return NULL;//MALLOC FAIL
    }
    for (int i=0;i< getLineNumInFile(queues);i++){
        putNextLineToString(queues,tempStr);
        char *token= strtok(tempStr," ");
        Course tempCourse=findCourseByID(sys->m_coursesList,atoi(token));
        token= strtok(NULL," ");
        while (token!=NULL){
            Student tempStudent= findStudentByID(sys->m_studentsList,atoi(token));
            IsraeliQueueEnqueue(tempCourse->m_queue,tempStudent);
            token= strtok(NULL," ");
        }
    }
    return sys;
}

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
Student *createStudentListFromFile(FILE* students, int *length) {
    assert(students != NULL);
    int maxNameSize = getMaxWordInFile(students), lines = getLineNumInFile(students);
    char *tempStr = (char *) malloc(sizeof(char) * getMaxLineInFile(students));
    if (!tempStr) {
        return NULL;//MALLOC FAIL
    }
    Student *studentList = (Student*) malloc((sizeof(Student)) * (lines + 1));
    if (!studentList) {
        return NULL;//MALLOC FAIL
    }
    for (int k = 0; k < lines; k++) {
        putNextLineToString(students, tempStr);
        studentList[k] = createStudentFromLine(tempStr, maxNameSize);
        if (!studentList[k]) {
            return NULL;
        }
    }
    length[0] = lines;
    studentList[lines] = NULL;
    return studentList;
}


Student createStudentFromLine(char *str, int maxWord){
    Student newStudent=(Student)malloc(sizeof(Student));
    if (!newStudent){
        return NULL;//MALLOC FAIL
    }
    char* tempStr=(char*)malloc(sizeof(char)*(maxWord+1)*2);
    if(!tempStr){
        free (newStudent);
        return NULL;
    }
    char *token= strtok(str, " ");

    int i=0;
    while(token!=NULL){
        switch (i++) {
            case ID_INDEX:
                newStudent->m_studentID= (int)strtol(token,&token ,10);
                break;
            case F_NAME_INDEX:
                strcpy(tempStr, token);
                strcat(tempStr," ");
                break;
            case L_NAME_INDEX:
                strcat(tempStr, token);
                break;
            default:
                break;
        }
        token= strtok(NULL, " ");
    }
    newStudent->m_name= (char*)malloc(sizeof(char)*(strlen(tempStr)+1));
    if(!newStudent->m_name){
        return NULL;// MALLOC FAIL
    }
    strcpy(newStudent->m_name,tempStr);
    free(tempStr);
    newStudent->m_enemiesList=NULL;
    newStudent->m_friendsList=NULL;

    return newStudent;
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
        courseList[i]->m_number= atoi(tempStr);
        putNextWordToString(courses,tempStr);
        courseList[i]->m_size= atoi(tempStr);
      //  courseList[i]->m_queue= IsraeliQueueCreate(NULL,isTheSameStudent,FRIENDSHIP_THR,RIVALRY_THR);
       // if (!courseList[i]->m_queue){
      //      return NULL;//CREATE FAIL
      //  }
        courseList[i]->m_queue=NULL;

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
Course findCourseByID(Course *courseList, int ID){
    assert(courseList);
    int i=0;
    while(courseList[i]!=NULL){
        if (courseList[i]->m_number==ID){
            return courseList[i];
        }
        i++;

    }
    return NULL;
}
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
