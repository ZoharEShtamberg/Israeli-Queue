#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "FileHelper.h"
#include "HackerEnrollment.h"
#include "../IsraeliQueue.h"



#define ID_INDEX 0
#define ID_LENGTH 9
#define F_NAME_INDEX 3
#define L_NAME_INDEX 4
#define RIVALRY_THR (-20)
#define FRIENDSHIP_THR 20
#define AMOUNT_OF_FF 3

#define MIN(A, B) ((A)<=(B)) ? (A) : (B)
//=================================================================
/*typedefs & struct declaration*/
//=================================================================


struct Course_t{
    int m_number, m_size;
    IsraeliQueue m_queue;
};

struct Student_t{
    int m_studentID;
    char *m_name;
    int *m_friendsList, *m_enemiesList;
};

struct Hacker_t{
    int  *m_preferredCourses;
    int m_preferredCoursesSize;
    Student m_studentCard;

};

struct EnrollmentSystem_t{
    Student *m_studentsList;
    Hacker  *m_hackersList;
    Course *m_coursesList;
    int m_studentsNum, m_coursesNum, m_hackersNum;
    //int **m_queues;
    FriendshipFunction *m_functionArray;
};

//=================================================================================
//Inner Functions Declarations:
//=================================================================================

Student *createStudentListFromFile(FILE* students, int *length);
Course *createCourseListFromFile(FILE* courses, int *length,FriendshipFunction *functionArray);
Hacker *createHackersListFromFile(FILE* hackers, int *length, const Student *studentList,int maxArrayNum);
Student createStudentFromLine(char* str,int maxWord);

void updateFriendshipFunctions(EnrollmentSystem sys);
void insertHackersToQueues(EnrollmentSystem sys);
Student areAllHackersSatisfied(EnrollmentSystem sys);
void printQueuesToFile(EnrollmentSystem sys, FILE* outFile);


Student findStudentByID(const Student *studentList, int ID);

Course findCourseByID(Course *courseList, int ID);



int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB);
int friendshipByASCII(void* ptrStudentA, void* ptrStudentB);
int friendshipByIDDiff(void* ptrStudentA, void* ptrStudentB);

void destroyStudentList(Student *List,int length);
void destroyCoursesList(Course *List,int length);
void destroyHackersList(Hacker *List,int length);


/*==================================================================================
 *LIBRARY FUNCTIONS: lord help
 ==================================================================================
 */

/**
 * literally Destroys everything
 */
void destroyEnrollment(EnrollmentSystem sys){
    if(!sys){
        return;//is this an error?
    }
    destroyHackersList(sys->m_hackersList,sys->m_hackersNum);
    destroyCoursesList(sys->m_coursesList,sys->m_coursesNum);
    destroyStudentList(sys->m_studentsList,sys->m_studentsNum);

    free(sys->m_functionArray);
    free(sys);
}




/**Creates a new Enrollment system based on the files provided
 * FriendshipFunction array is initialized to null.
 * returns a pointer to the function-memory must be freed by destroyEnrollment
 * in case of failure returns null-memory handled in function
 * */

EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){
   if(!students||!courses||!hackers){
       return NULL;//BAD PARAM
   }
    EnrollmentSystem newSys=(EnrollmentSystem)malloc(sizeof(EnrollmentSystem));
    if (!newSys){
        return NULL;//MALLOC FAIL
    }
    int counter=0;

    newSys->m_studentsList= createStudentListFromFile(students,&counter);
    if(!newSys->m_studentsList){
        destroyEnrollment(newSys);
        return NULL;//MALLOC FAIL
    }
   newSys->m_studentsNum=counter;
    newSys->m_functionArray= (FriendshipFunction *)malloc(sizeof (FriendshipFunction)*AMOUNT_OF_FF);
    if(!newSys->m_functionArray){
        destroyEnrollment(newSys);
        return NULL;//MALLOC FAIL
    }
    for(int i =0;i<AMOUNT_OF_FF;i++){
        newSys->m_functionArray[i]=NULL;
    }
    newSys->m_coursesList= createCourseListFromFile(courses,&newSys->m_coursesNum, newSys->m_functionArray);
    if(!newSys->m_coursesList){
        destroyEnrollment(newSys);
        return NULL;//CREATE FAIL
    }
    int maxArr=max(newSys->m_coursesNum,newSys->m_studentsNum);
    newSys->m_hackersList= createHackersListFromFile(hackers,&(newSys->m_hackersNum),newSys->m_studentsList, maxArr);
    if (!newSys->m_hackersList){
        destroyEnrollment(newSys);
        return NULL;//CREATE FAIL
    }
    return newSys;
}

/**
 * Reads and fills in relevant queues to courses provided by file,
 * in case of error returns null
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues){
    assert(sys&&queues);
    char* tempStr=(char*)malloc(sizeof(char)* getMaxWordInFile(queues+1));
    if (!tempStr){
        return NULL;//MALLOC FAIL
    }
    for (int i=0;i< getLineNumInFile(queues);i++){
        putNextLineToString(queues,tempStr);
        char *token= strtok(tempStr," ");
        Course tempCourse=findCourseByID(sys->m_coursesList,(int)strtod(token,&token));
        token= strtok(NULL," ");
        while (token!=NULL){
            Student tempStudent= findStudentByID(sys->m_studentsList,(int)strtod(token,&token));
            IsraeliQueueEnqueue(tempCourse->m_queue,tempStudent);
            token= strtok(NULL," ");
        }
    }
    return sys;
}

/**
 * updates queues based on friendship functions and hackers
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out){
    if (!sys||!out){
        return;//BAD PARAM
    }
    updateFriendshipFunctions(sys);
    insertHackersToQueues(sys);
    Student firstHackerUnsatisfied= areAllHackersSatisfied(sys);
    if(firstHackerUnsatisfied){
        fprintf(out, "Cannot satisfy constraints for %*d\n",ID_LENGTH,firstHackerUnsatisfied->m_studentID);
        return;
    }
    printQueuesToFile(sys, out);
    }

/**:
 *returns 1 if equal and 0 if not
 * */
int isTheSameStudent(void* stuA, void* stuB){
    Student student1=(Student)stuA, student2=(Student)stuB;
    if(student1->m_studentID==student2->m_studentID){
        return 1;
    }
    return 0;
}


/**
 * switches names to lower case
 * @param sys must have filled in students list
 */

void ignoreUpper(EnrollmentSystem sys){
    for (int k = 0; k < sys->m_studentsNum; k++)
    {
        int len = (int)strlen(sys->m_studentsList[k]->m_name);
        for (int i = 0; i < len; i++) {
            if (sys->m_studentsList[k]->m_name[i] >= 'A' && sys->m_studentsList[k]->m_name[i] <= 'Z') {
                sys->m_studentsList[k]->m_name[i] =(char) (sys->m_studentsList[k]->m_name[i] + ('a' - 'A'));
            }
        }
    }
}


/**========================================================================
 *Inner Functions
 *=========================================================================
*/


void destroyStudentList(Student *List,int length){
    if(!List){
        return;
    }
    for(int i=0;i<length;i++){
        if(List[i]){
            free(List[i]->m_name);
            free(List[i]->m_enemiesList);
            free(List[i]->m_friendsList);
            free(List[i]);
            }
        }
    free(List);
}

void destroyCoursesList(Course *List,int length){
    if(!List){
        return;
    }
    for(int i=0;i<length;i++){
        if(List[i]){
            if(List[i]->m_queue) {
                IsraeliQueueDestroy(List[i]->m_queue);
            }
        }
    }
    free(List);
}

void destroyHackersList(Hacker *List,int length){
    if(!List){
        return;
    }
    for(int i=0;i<length;i++){
        if(List[i]){
            free(List[i]->m_preferredCourses);
            free(List[i]);
        }
    }
    free(List);
}


/**
 * prints system queues as they are to file
 */
void printQueuesToFile(EnrollmentSystem sys, FILE* outFile){
    if(!sys||!outFile){
        return;
    }
    for (int i = 0; i < sys->m_coursesNum; i++) {
        IsraeliQueue tempQueue= IsraeliQueueClone(sys->m_coursesList[i]->m_queue);
        assert(tempQueue);
        fprintf(outFile,"%d", sys->m_coursesList[i]->m_number);
        Student tempStudent= IsraeliQueueDequeue(tempQueue);
        while(tempStudent){
            fprintf(outFile, " %*d",ID_LENGTH, tempStudent->m_studentID);
            tempStudent= IsraeliQueueDequeue(tempQueue);
        }
        fprintf(outFile, "\n");
    }
}


/**
 * this adds the 3 friendship functions to all the queues in sys
 */
void updateFriendshipFunctions(EnrollmentSystem sys) {
    for (int i = 0; i < sys->m_coursesNum; i++) {
        if (sys->m_coursesList[i]->m_queue) {
            IsraeliQueueAddFriendshipMeasure(sys->m_coursesList[i]->m_queue, friendshipByASCII);
            IsraeliQueueAddFriendshipMeasure(sys->m_coursesList[i]->m_queue, friendshipByHackerFile);
            IsraeliQueueAddFriendshipMeasure(sys->m_coursesList[i]->m_queue, friendshipByIDDiff);
        }
    }
}


/**
 * this function inserts hackers found in Enrollment system to their preferred courses
 */

void insertHackersToQueues(EnrollmentSystem sys){
    assert(sys);
    for (int i=0; i<sys->m_hackersNum;i++) {
        for (int k = 0; k < sys->m_hackersList[i]->m_preferredCoursesSize; k++) {
            Course tempCourse = findCourseByID(sys->m_coursesList, sys->m_hackersList[i]->m_preferredCourses[k]);
            assert(tempCourse);
            IsraeliQueueEnqueue(tempCourse->m_queue,sys->m_hackersList[i]->m_studentCard);
        }
    }

}


/**
 * @param sys a fully created enrollment system
 * @return student card of the first hacker that isn't satisfied
 */
Student areAllHackersSatisfied(EnrollmentSystem sys){
    assert(sys);
    for (int i=0; i<sys->m_hackersNum;i++){
        int failCount=0;
        for (int k=0;k<sys->m_hackersList[i]->m_preferredCoursesSize;k++){
            Course tempCourse = findCourseByID(sys->m_coursesList, sys->m_hackersList[i]->m_preferredCourses[k]);
            assert(tempCourse);
            IsraeliQueue tempQueue= IsraeliQueueClone(tempCourse->m_queue);
            assert(tempQueue);
            Student tempStudent= IsraeliQueueDequeue(tempQueue);
            int queueIndex=0;
            bool foundFlag=false;

            while(tempStudent){
                queueIndex++;
                if(tempStudent->m_studentID==sys->m_hackersList[i]->m_studentCard->m_studentID){
                    foundFlag=true;

                    break;
                }
                tempStudent= IsraeliQueueDequeue(tempQueue);
            }
            assert(foundFlag);
            if(queueIndex>tempCourse->m_size){
                failCount++;
            }
            IsraeliQueueDestroy(tempQueue);
        }
        if(failCount > MIN(2, sys->m_hackersList[i]->m_preferredCoursesSize)){
            return sys->m_hackersList[i]->m_studentCard;
        }    
    }
    return NULL;
}


/**
 * creates the student array
 *
 * @param students
 * @param length where to store the length of array
 * @return array list of all students in file
 * in case of failure returns NULL
 * user is responsible for freeing malloc
 */
Student *createStudentListFromFile(FILE* students, int *length) {
    assert(students != NULL);
    int maxNameSize = getMaxWordInFile(students), lines = getLineNumInFile(students);
    length[0] = lines;
    char *tempStr = (char *) malloc(sizeof(char) * getMaxLineInFile(students));
    if (!tempStr) {
        return NULL;//MALLOC FAIL
    }
    Student *studentList = (Student*) malloc((sizeof(Student)) * (lines + 1));
    if (!studentList) {
        free(tempStr);
        return NULL;//MALLOC FAIL
    }
    for (int k = 0; k < lines; k++) {
        putNextLineToString(students, tempStr);
        studentList[k] = createStudentFromLine(tempStr, maxNameSize);
        if (!studentList[k]) {
            free(tempStr);
            destroyStudentList(studentList,k+1);
            return NULL;
        }
    }

    free(tempStr);
    studentList[lines] = NULL;
    return studentList;
}

/**
 * creates student from line
 * @param str a line describing student info
 * @param maxWord mas name length
 * @return pointer to student
 *  in case of failure returns NULL
 * user is responsible for freeing malloc
 */
Student createStudentFromLine(char *str, int maxWord){
    Student newStudent=(Student)malloc(sizeof(struct Student_t));
    if (!newStudent){
        return NULL;//MALLOC FAIL
    }

    char* tempStr=(char*)malloc(sizeof(char)*(maxWord+1)*2);
    if(!tempStr){
        free (newStudent);
        return NULL;
    }


    char *token=strtok(str, " ");
    int i=0;
    while(i<=L_NAME_INDEX){
        switch (i++) {
            case ID_INDEX:
                newStudent->m_studentID= (int)strtod(token, &token);
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

    newStudent->m_name= (char*)malloc(sizeof(char)*(strlen(tempStr)+2));
    if(!newStudent->m_name){
        free(newStudent);
        free(tempStr);
        return NULL;// MALLOC FAIL
    }
    strcpy(newStudent->m_name,tempStr);
    newStudent->m_enemiesList=NULL;
    newStudent->m_friendsList=NULL;

    free(tempStr);

    return newStudent;
}

/**
 * creates course list
 * @param courses
 * @param length
 * @param functionArray
 * @return pointer to course list
 * *  in case of failure returns NULL
 * user is responsible for freeing malloc
 */
Course *createCourseListFromFile(FILE* courses, int *length, FriendshipFunction *functionArray){
    assert(courses!=NULL);
    int maxWord= getMaxWordInFile(courses),lines= getLineNumInFile(courses);
    Course *courseList=(Course*)malloc(sizeof(struct Course_t)*(lines+1));
    if (!courseList){
        return NULL;//MALLOC FAIL
    }
    char *tempStr=(char*) malloc(sizeof (char)*(maxWord*2+2));
    if(!tempStr){
        free(courseList);
        return NULL;//MALLOC FAIL
    }

    for (int i=0;i<lines;i++){
        courseList[i]=(Course)malloc(sizeof(Course));
        if(!courseList[i]){
            destroyCoursesList(courseList,i);
            free (tempStr);
            return NULL;//MALLOC FAIL
        }
        putNextWordToString(courses,tempStr);
        courseList[i]->m_number= atoi(tempStr);
        putNextWordToString(courses,tempStr);
        courseList[i]->m_size=atoi(tempStr);
        courseList[i]->m_queue= IsraeliQueueCreate(functionArray,isTheSameStudent,FRIENDSHIP_THR,RIVALRY_THR);
        if (!courseList[i]->m_queue){
            destroyCoursesList(courseList,i);
            free(tempStr);
            return NULL;//CREATE FAIL
        }
        courseList[i]->m_queue=NULL;
        strcpy(tempStr," ");
    }
    free(tempStr);
    length[0]=lines;
    return courseList;
}

/**
 *
 * @param hackers file containing hacker inf0
 * @param length where to store amount of hackers
 * @param studentList filled in student list
 * @param maxArrayNum maximum array size for friends and courses lists
 * @return pointer hacker list
 *  in case of failure returns NULL
 * user is responsible for freeing malloc
 */
Hacker *createHackersListFromFile(FILE* hackers, int *length, const Student *studentList, int maxArrayNum){
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
        hackerList[i]=(Hacker)malloc(sizeof (struct Hacker_t));
        if(!hackerList[i]){
            return NULL;//MALLOC FAIL;
        }

      //  hackerList[i]->m_studentCard= (Student )malloc(sizeof (Student));

        //if(!hackerList[i]->m_studentCard){
          //  return NULL;//couldn't find student;
      //  }
        hackerList[i]->m_studentCard= findStudentByID(studentList, (int)strtol(tempStr,&tempStr,10));
        //parse line to courses:
        putNextLineToString(hackers,tempStr);
        hackerList[i]->m_preferredCourses= fillIntArrayFromStr(tempArray,tempStr);
        if(!hackerList[i]->m_preferredCourses){
            return NULL; //MALLOC FAIL
        }
        resetIntArray(tempArray,maxArrayNum);

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

/**
 * finds relevant course and returns a pointer to it
 */
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


/**
 * finds relevant student and returns a pointer to it
 */
Student findStudentByID(const Student *studentList, int ID){
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


/**
 * swaps two students
 */
void swap(Student *studentA,Student *studentB){
    Student temp1=*studentA;
    *studentA=*studentB;
    *studentB=temp1;
}


/**
*=================================================================================
*FRIENDSHIP FUNCTIONS: they really explain themselves...
*==================================================================================
*/


int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int i=0;
    if (studentA->m_friendsList==NULL){
        swap(&studentA,&studentB);
    }
    assert(studentA->m_friendsList);
    while(studentA->m_friendsList[i]!=0){
        if (studentA->m_friendsList[i++]==(studentB->m_studentID)){
            free(studentA);
            free(studentB);
            return FRIENDSHIP_THR;
        }
    }
    i=0;
    while(studentA->m_enemiesList[i]!=0){
        if (studentA->m_enemiesList[i++]==(studentB->m_studentID)){
            free(studentA);
            free(studentB);
            return RIVALRY_THR;
        }
    }
    return 0;
}


int friendshipByASCII(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int stuANameLen=(int)strlen(studentA->m_name), stuBNameLen=(int)strlen(studentB->m_name), sum=0;
    if (stuANameLen<stuBNameLen){
        swap(&studentA,&studentB);
    }
    for (int i=0;i<stuBNameLen;i++){
        int tempValue=((int)(studentA->m_name[i])-(int)(studentB->m_name[i]));
        if(tempValue<0){
            tempValue*=(-1);
        }
        sum+=tempValue;
    }
    for (int i=stuBNameLen;i<stuANameLen;i++)
        sum+=(int)studentA->m_name[i];

    return sum;
}


int friendshipByIDDiff(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int diff=studentA->m_studentID-studentB->m_studentID;
    if (diff<0){
        diff*=(-1);
    }
    return diff;
}


