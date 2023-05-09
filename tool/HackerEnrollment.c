#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "FileHelper.h"
#include "HackerEnrollment.h"
#include "../IsraeliQueue.h"
//TODO not suppose to be


#define ID_INDEX 0
#define ID_LENGTH 9
#define F_NAME_INDEX 3
#define L_NAME_INDEX 4
#define RIVALRY_THR (-20)
#define FRIENDSHIP_THR 20
#define AMOUNT_OF_FF 3

#define MIN(A, B) ((A)<=(B)) ? (A) : (B)
#define MAX(A, B) ((A)>=(B)) ? (A) : (B)


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
    int m_friendsNum, m_enemiesNum;
};

struct Hacker_t{
    int* m_preferredCourses;
    int m_coursesNum;
    Student m_studentCard;

};

struct EnrollmentSystem_t{
    Student *m_studentsList;
    Hacker  *m_hackersList;
    Course *m_coursesList;
    int m_studentsNum, m_coursesNum, m_hackersNum;
    FriendshipFunction *m_functionArray;
};

//=====================================================================
//Inner Functions Declarations:
//===================================================================

Student *createStudentListFromFile(FILE* students, int *length);
Student createStudentFromLine(char* str,int maxWord);
Course *createCourseListFromFile(FILE* courses, int *length,FriendshipFunction *functionArray);
Hacker *createHackersListFromFile(FILE* hackers, EnrollmentSystem newSys);
Hacker createHacker(FILE *hackers, EnrollmentSystem sys, int maxSize);

void destroyStudentList(Student *List,int length);
void destroyCoursesList(Course *List,int length);
void destroyHackersList(Hacker *List,int length);

Student findStudentByID(const Student *studentList, int ID, int size);
Course findCourseByID(Course *courseList, int ID, int size);

void updateFriendshipFunctions(EnrollmentSystem sys);
bool insertHackersToQueues(EnrollmentSystem sys);
Student areAllHackersSatisfied(EnrollmentSystem sys);
void printQueuesToFile(EnrollmentSystem sys, FILE* outFile);


int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB);
int friendshipByASCII(void* ptrStudentA, void* ptrStudentB);
int friendshipByIDDiff( void* ptrStudentA, void* ptrStudentB);
int calculateDiffInASCII(const char *a,const char *b);

/*=======================================================================
 *LIBRARY FUNCTIONS: lord help
 ========================================================================*/



/**Creates a new Enrollment system based on the files provided
 * FriendshipFunction array is initialized to null.
 * returns a pointer to the function-memory must be freed by destroyEnrollment
 * in case of failure returns null-memory handled in function
 * */

EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers){
   if(!students||!courses||!hackers){
       return NULL;//BAD PARAM
   }
    EnrollmentSystem newSys=(EnrollmentSystem)malloc(sizeof(*newSys));
    if (!newSys){
        return NULL;//MALLOC FAIL
    }

    newSys->m_studentsList= createStudentListFromFile(students,&newSys->m_studentsNum);
    if(!newSys->m_studentsList){
        destroyEnrollment(newSys);
        return NULL;//MALLOC FAIL
    }

    newSys->m_functionArray= (FriendshipFunction *)malloc(sizeof (FriendshipFunction)*AMOUNT_OF_FF);
    if(!newSys->m_functionArray){
        destroyEnrollment(newSys);
        return NULL;//MALLOC FAIL
    }
    for(int i =0;i<AMOUNT_OF_FF;i++){// initializing functions to NULL
        newSys->m_functionArray[i]=NULL;
    }

    newSys->m_coursesList= createCourseListFromFile(courses,&newSys->m_coursesNum, newSys->m_functionArray);
    if(!newSys->m_coursesList){
        destroyEnrollment(newSys);
        return NULL;//CREATE FAIL
    }

    newSys->m_hackersList= createHackersListFromFile(hackers,newSys);
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
    unsigned long maxLine= getMaxLineInFile(queues)+1;
    char* tempStr=malloc(sizeof(char)* (maxLine));
    if (!tempStr){
        return NULL;//MALLOC FAIL
    }
	int linesInFile = getLineNumInFile(queues);
    for (int i=0;i < linesInFile;i++){	//:)
        putLineFromFileInString(tempStr,queues);
        char *token= strtok(tempStr," ");
        Course tempCourse=findCourseByID(sys->m_coursesList,atoi(token),sys->m_coursesNum);
        assert(tempCourse);
        token= strtok(NULL," ");
        while (token!=NULL){
            Student tempStudent= findStudentByID(sys->m_studentsList,atoi(token),sys->m_studentsNum);
            IsraeliQueueEnqueue(tempCourse->m_queue,tempStudent);
            token= strtok(NULL," ");
        }
    }
    free(tempStr);
    return sys;
}

/**
 * updates queues based on friendship functions and hackers
 *
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out){
    if (!sys||!out){
        return;//BAD PARAM
    }
    updateFriendshipFunctions(sys);
    if(!insertHackersToQueues(sys)){
        return;
    }
    Student firstHackerUnsatisfied= areAllHackersSatisfied(sys);
    if(firstHackerUnsatisfied){
        printQueuesToFile(sys, out);//TODO
        fprintf(out, "Cannot satisfy constraints for %*d\n",ID_LENGTH,firstHackerUnsatisfied->m_studentID);
        return;
    }
    printQueuesToFile(sys, out);
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


/**:
 *returns 1 if equal and 0 if not
 * */
int isTheSameStudent(void* ptrA, void* ptrB){
    Student student1=(Student)ptrA, student2=(Student)ptrB;
    if(student1->m_studentID==student2->m_studentID){
        return 1;
    }
    return 0;
}


/**========================================================================
 *Inner Functions
 *=========================================================================*/

//=======================================================
//DESTROY FUNCTIONS:
//=======================================================

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
            free(List[i]);
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
 * literally Destroys everything it can
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


//=======================================================
//CREATE FUNCTIONS:
//=======================================================

/**
 * creates the student array
 *
 * @param students
 * @param length where to store the length of array
 * @return array list of all students in file
 * in case of failure returns NULL
 * doesnt destroy memory allocations that belong to new sys upon failure-destroy function responsibility
 */
Student *createStudentListFromFile(FILE* students, int *length) {
    assert(students != NULL);
    int maxNameSize = getMaxWordInFile(students), lines = getLineNumInFile(students);
    *length = lines;
    unsigned long maxLineSize=getMaxLineInFile(students);
    char *tempStr = malloc(sizeof(char) * (maxLineSize+1));
    if (!tempStr) {
        return NULL;//MALLOC FAIL
    }
    Student *studentList = malloc((sizeof(Student)) * (lines ));
    if (!studentList) {
        free(tempStr);
        return NULL;//MALLOC FAIL
    }
    for (int k = 0; k < lines; k++) {
        putLineFromFileInString(tempStr,students );
        studentList[k] = createStudentFromLine(tempStr, maxNameSize);
        if (!studentList[k]) {
            free(tempStr);
            destroyStudentList(studentList,k+1);
            return NULL;
        }
    }

    free(tempStr);
    return studentList;
}

/**
 * creates student from line
 * @param str a line describing student info
 * @param maxWord mas name length
 * @return pointer to student
 *  in case of failure returns NULL
 * u * doesnt destroy memory allocations that belong to new sys upon failure-destroy function responsibility
 */
Student createStudentFromLine(char *str, int maxWord){
    Student newStudent=(Student)malloc(sizeof(*newStudent));
    if (!newStudent){
        return NULL;//MALLOC FAIL
    }
    char* tempStr=malloc(sizeof(char)*(maxWord+1)*2);
    if(!tempStr){
        free (newStudent);
        return NULL;
    }
    char *token=strtok(str, " ");
    int i=0;
    while(i<=L_NAME_INDEX&&token){
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

    newStudent->m_name= malloc(sizeof(char)*(strlen(tempStr)+1));
    if(!newStudent->m_name){
        free(newStudent);
        free(tempStr);
        return NULL;// MALLOC FAIL
    }
    strcpy(newStudent->m_name,tempStr);
    newStudent->m_enemiesList=NULL;
    newStudent->m_friendsList=NULL;
    newStudent->m_enemiesNum=0;
    newStudent->m_friendsNum=0;
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
 *  * doesnt destroy memory allocations that belong to new sys upon failure-destroy function responsibility
 */
Course *createCourseListFromFile(FILE* courses, int *length, FriendshipFunction *functionArray){
    assert(courses);
    int lines= getLineNumInFile(courses);
    unsigned long maxStrLen= getMaxLineInFile(courses)+1;

    Course *courseList=(Course*)malloc(sizeof(Course)*lines);
    if (!courseList){
        return NULL;//MALLOC FAIL
    }
    char *tempStr= malloc(sizeof (char)*maxStrLen);
    if(!tempStr){
        free(courseList);
        return NULL;//MALLOC FAIL
    }

    for (int i=0;i<lines;i++){
        putLineFromFileInString(tempStr,courses);
        if(strlen(tempStr)<=1){
            i--;
            continue;
        }
        courseList[i]=(Course)malloc(sizeof(*courseList[i]));
        if(!courseList[i]){
            destroyCoursesList(courseList,i);
            free (tempStr);
            return NULL;//MALLOC FAIL
        }

        int check=sscanf(tempStr,"%d %d",&courseList[i]->m_number, &courseList[i]->m_size);
        assert(check==2);//if fail wrong format or question parameters
        courseList[i]->m_queue= IsraeliQueueCreate(functionArray,isTheSameStudent,FRIENDSHIP_THR,RIVALRY_THR);
        if (!courseList[i]->m_queue){
            destroyCoursesList(courseList,i);
            free(tempStr);
            return NULL;//CREATE FAIL
        }
    }
    free(tempStr);
    *length=lines;
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
 *  destroys itself upon failure in create
 */
Hacker *createHackersListFromFile(FILE* hackers, EnrollmentSystem newSys){
    assert(hackers&&newSys);

    newSys->m_hackersNum=getLineNumInFile(hackers)/4;

    Hacker *hackerList=malloc(sizeof(Hacker)*(newSys->m_hackersNum));
    if(!hackerList){
        return NULL;//MALLOC FAIL
    }


    int bufferSize=getMaxLineInFile(hackers)+1;
    for (int i=0;i<(newSys->m_hackersNum);i++){
        hackerList[i]= createHacker(hackers,newSys,bufferSize);
        if(!hackerList[i]){
            destroyHackersList(hackerList,i);
            return NULL;
        }
    }
    return hackerList;
}


/**creates a new hacker object from the next hacker data in file according to format
 *in case of failure returns null
 * doesnt destroy memory allocations that belong to new sys upon failure-destroy function responsibility
 */
Hacker createHacker(FILE *hackers, EnrollmentSystem sys, int maxSize){
    if(!hackers||!sys){
        return NULL;//BAD PARM
    }

    char *tempStr=malloc(sizeof(char)* (maxSize+1) );
    if (!tempStr){
        return NULL;//MALLOC FAIL
    }
    Hacker newHacker=malloc(sizeof (*newHacker));

    if(!newHacker){
        free(tempStr);
        return NULL;//MALLOC FAIL;
    }
    putLineFromFileInString(tempStr,hackers);
    newHacker->m_studentCard= findStudentByID(sys->m_studentsList, atoi(tempStr),sys->m_studentsNum);
    assert(newHacker->m_studentCard);//hacker must be in student file
    //parse line to courses array:

    if(putLineFromFileInString(tempStr,hackers)==0){
        newHacker->m_preferredCourses= NULL;
    }
    else{
        newHacker->m_preferredCourses = createIntArrayFromStr(tempStr, &newHacker->m_coursesNum);
        if (!newHacker->m_preferredCourses){
            free(tempStr);
            free(newHacker);
            return NULL;
        }
    }
    //parse line to friends array:
    if(putLineFromFileInString(tempStr,hackers)==0){
        newHacker->m_studentCard->m_friendsList=NULL;
    } else{
        newHacker->m_studentCard->m_friendsList=createIntArrayFromStr(tempStr,&newHacker->m_studentCard->m_friendsNum);
        if(!newHacker->m_studentCard->m_friendsList){
            free(tempStr);
            free(newHacker->m_preferredCourses);
            free(newHacker);
            return NULL; //MALLOC FAIL
        }
    }
    //parse line to enemies array:
    if(putLineFromFileInString(tempStr,hackers)==0){
        newHacker->m_studentCard->m_enemiesList=NULL;
    }
    else {
        newHacker->m_studentCard->m_enemiesList=createIntArrayFromStr(tempStr,&newHacker->m_studentCard->m_enemiesNum);
        if(!newHacker->m_studentCard->m_enemiesList){
            free(newHacker->m_studentCard->m_friendsList);
            free(newHacker->m_preferredCourses);
            free(newHacker);
            free(tempStr);
            return NULL; //MALLOC FAIL
        }
    }
    free(tempStr);
    return newHacker;
}


//=======================================================
//READ AND HACK FUNCTIONS
//=======================================================
/**
 * finds relevant course and returns a pointer to it, NULL if not found in List
 */
Course findCourseByID(Course *courseList, int ID,int size){
    assert(courseList);
    for(int i=0;i<size;i++){
        if (courseList[i]->m_number==ID){
            return courseList[i];
        }
    }
    return NULL;
}


/**
 * finds relevant student and returns a pointer to it, NULL if not found in List
 */
Student findStudentByID(const Student *studentList, int ID, int size){
    if(!studentList){
        return NULL;//BAD PARAM
    }
    for (int i=0;i<size;i++){
        if(studentList[i]->m_studentID==ID){
            return studentList[i];
        }

    }
    return NULL;
}


/**
 * prints system queues as they are to file, only prints non-empty queues
 */
void printQueuesToFile(EnrollmentSystem sys, FILE* outFile){
    if(!sys||!outFile){
        return;
    }

    for (int i = 0; i < sys->m_coursesNum; i++) {
        IsraeliQueue tempQueue= IsraeliQueueClone(sys->m_coursesList[i]->m_queue);

        assert(tempQueue);

        if(IsraeliQueueSize(tempQueue)==0){
            IsraeliQueueDestroy(tempQueue);
            continue;
        }
        fprintf(outFile,"%d", sys->m_coursesList[i]->m_number);

        Student tempStudent= IsraeliQueueDequeue(tempQueue);

        while(tempStudent){
            fprintf(outFile, " %*d",ID_LENGTH, tempStudent->m_studentID);
            tempStudent= IsraeliQueueDequeue(tempQueue);
        }

        fprintf(outFile, "\n");
        IsraeliQueueDestroy(tempQueue);
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

bool insertHackersToQueues(EnrollmentSystem sys){
    assert(sys);
    for (int i=0; i<sys->m_hackersNum;i++) {
        if(sys->m_hackersList[i]->m_preferredCourses==0){
            continue;
        }
        for (int j = 0; j < sys->m_hackersList[i]->m_coursesNum; j++) {
            Course tempCourse = findCourseByID(sys->m_coursesList, sys->m_hackersList[i]->m_preferredCourses[j],sys->m_coursesNum);
            assert(tempCourse);
            if (IsraeliQueueEnqueue(tempCourse->m_queue,sys->m_hackersList[i]->m_studentCard)!=ISRAELIQUEUE_SUCCESS) {
                return false;
            }
        }
    }
    return true;

}


/**
 * @param sys a fully created enrollment system
 * @return student card of the first hacker that isn't satisfied
 * NULL upon successful hacking-no hackers unsatisfied  :)
 */
Student areAllHackersSatisfied( EnrollmentSystem sys){
    assert(sys);
    //hacker loop:
    for(int i=0;i<sys->m_hackersNum;i++){

        if(sys->m_hackersList[i]->m_preferredCourses==0){
            continue;
        } //jump if no wanted courses

        int failCount=0;
        int hackerId=sys->m_hackersList[i]->m_studentCard->m_studentID;
        //courses loop:
        for (int j=0; j < sys->m_hackersList[i]->m_coursesNum; j++) {

            int courseNumber=sys->m_hackersList[i]->m_preferredCourses[j], positionInLine=0;

            Course tempCourse = findCourseByID(sys->m_coursesList,courseNumber,sys->m_coursesNum );
            assert(tempCourse);
            IsraeliQueue tempQueue= IsraeliQueueClone(tempCourse->m_queue);
            assert(tempQueue);
            if (IsraeliQueueSize(tempQueue)<tempCourse->m_size){
                IsraeliQueueDestroy(tempQueue);
                continue;
            }

            Student tempStudent= IsraeliQueueDequeue(tempQueue);
            while(tempStudent){
                positionInLine++;
                if(tempStudent->m_studentID==hackerId){
                    break;
                }
                tempStudent= IsraeliQueueDequeue(tempQueue);
            }
            assert(positionInLine>0);

            if(positionInLine>=tempCourse->m_size){
                failCount++;
            }

            IsraeliQueueDestroy(tempQueue);
        }//check for fails

        if((sys->m_hackersList[i]->m_coursesNum)-failCount < 2||(failCount==sys->m_hackersList[i]->m_coursesNum)){
            return sys->m_hackersList[i]->m_studentCard;

        }
    }
    return NULL;
}


/**
*=================================================================================
*FRIENDSHIP FUNCTIONS: they really explain themselves...
*==================================================================================
*/

//checks two Student Types if either are friends/enemies of each-other
int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    const Student studentA=(Student)ptrStudentA;
    const Student studentB=(Student)ptrStudentB;
    int i=0;
    if(studentA->m_friendsList){
        for(;i<studentA->m_friendsNum;i++){
            if (studentA->m_friendsList[i]==(studentB->m_studentID)){
                return FRIENDSHIP_THR;
            }
        }
    }
    i=0;
    if(studentA->m_friendsList){
        for(;i<studentA->m_enemiesNum;i++){
            if (studentA->m_enemiesList[i]==(studentB->m_studentID)){
                return RIVALRY_THR;
            }
        }
    }
    i=0;
    //checks for both students
    if(studentB->m_friendsList){
        for(;i<studentB->m_friendsNum;i++){
            if (studentB->m_friendsList[i]==(studentA->m_studentID)){
                return 20;
            }
        }
    }
    i=0;
    if(studentB->m_friendsList){
        for(;i<studentB->m_enemiesNum;i++){
            if (studentB->m_enemiesList[i]==(studentA->m_studentID)){
                return -20;
            }
        }
    }
    return 0;
}

//evaluates the friendship of two Student Types based on ascii differences in names
int friendshipByASCII(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;

    int sum= calculateDiffInASCII(studentA->m_name,studentB->m_name);

    int firstNameALen= getNextWordLength(studentA->m_name),
        firstNameBLen= getNextWordLength(studentB->m_name);

    sum+=calculateDiffInASCII(&studentA->m_name[firstNameALen],&studentB->m_name[firstNameBLen]);
    return sum;
}

//evaluates the friendship of two Student Types based on ascii differences in names
int friendshipByIDDiff(void* ptrStudentA, void* ptrStudentB){
    assert(ptrStudentA&&ptrStudentB);
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int diff=studentA->m_studentID-studentB->m_studentID;
    if (diff<0){
        diff*=(-1);
    }
    return diff;
}


/**
 * calculates differences between characters in two strings
 * @param a,b strings-used for words with whitespaces or \0 or \n as delimiters
 * @returns the sum of the ascii differences word by word
 */
int calculateDiffInASCII(const char *a,const char *b){
    int sum=0, tempValue;
    int i=0;
    while( a[i]&&  b[i]  &&  a[i]!=' ' &&  b[i]!=' '&& a[i]!='\n'&& b[i]!='\n' ) {
        tempValue=a[i]-b[i];
        if(tempValue<0){
            tempValue*=(-1);
        }
        i++;
        sum+=tempValue;
    }

    if (a[i] && a[i]!=' ' && a[i]!='\n'){
        while(a[i] && a[i]!=' '){
            sum+=a[i++];
        }
    }
    else{
        while(b[i] && b[i]!=' '&& b[i]!='\n'){
            sum+=b[i++];
        }
    }
    return sum;
}