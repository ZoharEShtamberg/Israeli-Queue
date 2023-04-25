#include <stdio.h>
//#include <stdbool.h>
//#include <assert.h>
#include <stdlib.h>
#include "IsraeliQueue.h"
#include <string.h>


#define ID_LENGTH 10
#define STUDENT_TXT_ARG 7
#define ID_INDEX 0
#define F_NAME_INDEX 3
#define L_NAME_INDEX 4
#define RIVALRY_THR 20
#define FRIENDSHIP_THR 20
#define BIGGEST_INT_SIZE 9
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

void examineFileLinesAndSize(FILE* file, int *lines, int *maxWordLength);
Student findStudentByID(Student *studentList, int ID);
long int fileLength(FILE* file);
void copyIntArray(int src[],int dest[], int length);
int max(int a, int b);

//==================================================================================
//LIBRARY FUNCTIONS: lord help
//==================================================================================

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
    //check the students num
    newSys->m_studentsList= createStudentListFromFile(students,&(newSys->m_studentsNum));
    newSys->m_coursesList= createCourseListFromFile(courses,&(newSys->m_coursesNum));
    int maxArr=max(newSys->m_coursesNum,newSys->m_studentsNum);
    newSys->m_hackersList= createHackersListFromFile(hackers,&(newSys->m_hackersNum),newSys->m_studentsList, maxArr);

    newSys->m_functionArray= malloc(sizeof (FriendshipFunction)*AMOUNT_OF_FF);
    if (!newSys->m_functionArray){
        //TODO: destroy~~!!!!!
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

    if(!students){
        return NULL;//BAD PARAM
    }
    int maxNameSize,lines,ID;

    examineFileLinesAndSize(students,&lines,&maxNameSize);

    char *name=(char*)malloc(sizeof(char)*(maxNameSize+1)),
    *lastName=(char*)malloc(sizeof(char)*(maxNameSize+1)),
    *trashStr=(char*)malloc(sizeof(char)*(maxNameSize+1));
    if(!name||!lastName||!trashStr){
        free(name);
        free(lastName);
        free(trashStr);
        return NULL;//MALLOC FAIL
    }

    Student *studentList=(Student*)malloc((sizeof(Student))*(lines+1));
    if (!studentList){
        return NULL;//MALLOC FAIL
    }
    for(int k=0;k<lines;k++){
        for (int i=0;i<STUDENT_TXT_ARG;i++){
            switch (i){
                case ID_INDEX:
                    if(fscanf(students,"%d",&ID)!=1){
                        lines--;//BAD FILE
                    }
                    break;
                case F_NAME_INDEX:
                    fscanf(students,"%s",name);
                    break;
                case L_NAME_INDEX:
                    fscanf(students,"%s",lastName);
                    break;
                default:
                    fscanf(students,"%s",trashStr);
                    break;
            }
        }
        studentList[k]=(Student)malloc(sizeof(Student));
        if (studentList[k]){
            return NULL;//MALLOC FAIL
        }
        studentList[k]->m_studentID=ID;
        studentList[k]->m_name= (char*)malloc(sizeof(char)*(strlen(name)+strlen(lastName)+2));
        if(!studentList[k]->m_name){
            return NULL;// MALLOC FAIL
        }
        strcpy(studentList[k]->m_name,name);
        strcat(studentList[k]->m_name," ");
        strcat(studentList[k]->m_name,lastName);
        studentList[k]->m_enemiesList=NULL;
        studentList[k]->m_friendsList=NULL;
    }
    *length=lines;
    free(name);
    free(lastName);
    free(trashStr);
    studentList[lines]=NULL;
    return studentList;
}


Course *createCourseListFromFile(FILE* courses, int *length){
    if(!courses||!length){
        return NULL;//BAD PARAM
    }
    int maxWord,lines;
    examineFileLinesAndSize(courses,&lines,&maxWord);
    if(maxWord>BIGGEST_INT_SIZE){
        return NULL;//not compatible with question parameters;
    }

    int courseNum,courseSize;
    Course *courseList=(Course*)malloc(sizeof(Course)*(lines+1));
    if (!courseList){
        return NULL;//MALLOC FAIL
    }

    for (int i=0;i<lines;i++){
        if(fscanf(courses,"%d %d",&courseNum, &courseSize)!=2){
            break;
        }

        courseList[i]=(Course)malloc(sizeof(Course));
        if(!courseList[i]){
            return NULL;//MALLOC FAIL
        }

        courseList[i]->m_number=courseNum;
        courseList[i]->m_size=courseSize;
        courseList[i]->m_queue= IsraeliQueueCreate(NULL,isTheSameStudent,FRIENDSHIP_THR,RIVALRY_THR);
        if (!courseList[i]->m_queue){
            return NULL;//CREATE FAIL
        }

    }
    courseList[lines]=NULL;
    *length=lines;
    return courseList;
}


Hacker *createHackerListFromFile(FILE* hackers, int *length,Student *studentList, int maxArrayNum){
    if(!hackers||!length){
        return NULL;//BAD PARAM
    }

    int maxWord,lines;
    examineFileLinesAndSize(hackers,&lines,&maxWord);
    if(maxWord>BIGGEST_INT_SIZE){
        return NULL;//not compatible with question parameters;
    }

    Hacker *hackerList=(Hacker*) malloc(sizeof(Hacker)*((lines/4)+1));
    if(!hackerList){
        return NULL;//MALLOC FAIL
    }

    char *tempStr=(char*) malloc(sizeof(char)*maxWord);
    if (!tempStr){
        return NULL;//MALLOC FAIL
    }

    int hackersID; maxArrayNum++;
    int *preferredCourseTemp=(int*)malloc(sizeof(int)*maxArrayNum);
    int *enemiesTemp=(int*)malloc(sizeof(int)*maxArrayNum);
    int *friendsTemp=(int*)malloc(sizeof(int)*maxArrayNum);
    if(!preferredCourseTemp||!enemiesTemp||!friendsTemp){
        free(preferredCourseTemp);
        free(friendsTemp);
        free(enemiesTemp);
        return NULL;//MALLOC FAIL
    }

    for (int i=0;i<lines/4;i++){
        if(fscanf(hackers,"%d",&hackersID)!=1){
            continue;
        }

        int courseNum=0, friendsNum=0, enemiesNum=0, strIndex=0;
        char temp= (char)fgetc(hackers);

        while (temp!='\n'){
            while(temp!=' '&&temp!='\n'){
                tempStr[strIndex++]=temp;
                temp=(char)fgetc(hackers);
            }
            sscanf(tempStr,"%d",&preferredCourseTemp[courseNum++]);
            strcpy(tempStr,"");
            strIndex=0;
        }

        temp=(char)fgetc(hackers);

        while (temp!='\n'){
            while(temp!=' '&&temp!='\n'){
                tempStr[strIndex++]=temp;
                temp=(char)fgetc(hackers);
            }

            sscanf(tempStr,"%d",&friendsTemp[friendsNum++]);
            strcpy(tempStr,"");
            strIndex=0;
        }
        temp=(char)fgetc(hackers);

        while (temp!='\n'){
            while(temp!=' '&&temp!='\n'){
                tempStr[strIndex++]=temp;
                temp=(char)fgetc(hackers);
            }
            sscanf(tempStr,"%d",&enemiesTemp[enemiesNum++]);
            strcpy(tempStr,"");
            strIndex=0;
        }

        hackerList[i]=(Hacker)malloc(sizeof(Hacker));
        if(!hackerList[i]){
            return NULL;//MALLOC FAIL;
        }

        hackerList[i]->m_studentCard= findStudentByID(studentList,hackersID);
        if(!hackerList[i]->m_studentCard){
            return NULL;//couldn't find student;
        }

        hackerList[i]->m_preferredCourses=(int*) malloc(sizeof (int )*(courseNum+1));
        if(!hackerList[i]->m_preferredCourses){
            return NULL; //MALLOC FAIL
        }
        copyIntArray(preferredCourseTemp,hackerList[i]->m_preferredCourses, courseNum);
        hackerList[i]->m_preferredCourses[courseNum]=0;

        hackerList[i]->m_studentCard->m_friendsList=(int*) malloc(sizeof (int )*(friendsNum+1));
        if(!hackerList[i]->m_studentCard->m_friendsList){
            return NULL; //MALLOC FAIL
        }
        hackerList[i]->m_studentCard->m_friendsList[friendsNum]=0;
        copyIntArray(hackerList[i]->m_studentCard->m_friendsList,friendsTemp,friendsNum);

        hackerList[i]->m_studentCard->m_enemiesList=(int*) malloc(sizeof (int )*enemiesNum);
        if(!hackerList[i]->m_studentCard->m_enemiesList){
            return NULL; //MALLOC FAIL
        }
        copyIntArray(hackerList[i]->m_studentCard->m_enemiesList,enemiesTemp,enemiesNum);
        hackerList[i]->m_studentCard->m_enemiesList[enemiesNum]=0;

    }
    hackerList[lines/4 +1]=NULL;
    *length=lines/4;
    free(preferredCourseTemp);
    free(friendsTemp);
    free(enemiesTemp);
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
void copyIntArray(int src[],int dest[], int length){
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
//
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

int max(int a, int b){
    if (a>b)
        return a;
    return b;
}

void swap(Student studentA,Student studentB){
    Student temp1=studentA;
    studentA=studentB;
    studentB=temp1;
}


//=================================================================================
//FRIENDSHIP FUNCTIONS: they really explain themselves...
//==================================================================================


//TODO: rewrite this whole thing checks if 
int friendshipByHackerFile(void* ptrStudentA, void* ptrStudentB){
    if(!ptrStudentA||!ptrStudentB){
        return -1;//BAD PARAM
    }
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;

    int i=0;
    if (studentA->m_friendsList==NULL){
        swap(studentA,studentB);
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


//func desc: ill let you know when I find out
int friendshipByASCII(void* ptrStudentA, void* ptrStudentB){
    if(!ptrStudentA||!ptrStudentB){
        return -1;//BAD PARAM
    }
    Student studentA=(Student)ptrStudentA, studentB=(Student)ptrStudentB;
    int stuANameLen=strlen(studentA->m_name), stuBNameLen=strlen(studentB->m_name), sum=0;
    if (stuANameLen<stuBNameLen){
        swap(studentA,studentB);
    }
    for (int i=0;i<stuBNameLen;i++){
        sum+= abs((int)(studentA->m_name[i]-studentB->m_name[i]));
    }
    for (int i=stuBNameLen;i<stuANameLen;i++)
        sum+=(int)studentA->m_name[i];

    return sum;
}


//func desc: ill let you know when I find out
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

/*
int main(){

    FILE* students=fopen("C:\\Users\\Daniel\\Desktop\\ex1\\ExampleTest\\courses.txt", "r");
    if (!students){
        return 0;
    }
    int stuLength=0;
    Student *studentList=createStudentListFromFile(students,&stulength)!=NULL){
        printf("did it work?\n");
    }

    FILE* hackers=fopen("C:\\Users\\Daniel\\Desktop\\ex1\\ExampleTest\\courses.txt", "r");
    if (!hackers){
        return 0;
    }
    int hackLength=0;
    if(createHackerListFromFile(students,&hackLength,studentList)!=NULL){
        printf("did it work?\n");
    }

    FILE* courses=fopen("C:\\Users\\Daniel\\Desktop\\ex1\\ExampleTest\\courses.txt", "r");
    if (!courses){
        return 0;
    }
    int corLength=0;
    if(createCourseListFromFile(students,&corLength)!=NULL){
        printf("did it work?\n");
    }

    return 0;

}
 */