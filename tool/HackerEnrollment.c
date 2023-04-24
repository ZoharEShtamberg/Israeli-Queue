#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "IsraeliQueue.h"
#include <string.h>


#define ID_LENGTH 10
#define MAX_STRING_LEN 100
#define COURSE_NUM_LENGTH 6

//=================================================================
/*typedefs & struct declaration*/
//=================================================================
typedef struct Node_t{
	void *m_item;
	struct node_t *m_next;
} *Node;

typedef struct EnrollmentSystem_t{

    Student m_studentsHeadNode;
    Course m_coursesHeadNode;

    //int m_studentsNum, m_coursesNum, m_hackersNum;
    //do we need this? i really dont know i miss arrays what is this node shit

}*EnrollmentSystem;

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

//=================================================================================
//Inner Functions Declerations:
//=================================================================================

int fileLength(FILE* file);
Node* createStudentListFromFile(FILE* studentsFile);
Node* createSCoursesListFromFile(FILE* coursesFile);


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

    EnrollmentSystem newSys=malloc(sizeof(EnrollmentSystem));
    Node studentsListHead=createStudentListFromsFile(students);
    Node coursesListHead=createSCoursesListFromFile(courses);

    if (!newSys||!studentsListHead||!coursesListHead){
        return NULL;//MALLOC FAIL
    }
    /*
    code: update students by hackers file
    */

    newSys->m_coursesHeadNode=coursesListHead;
    newSys->m_studentsHeadNode=studentsListHead;   
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


//reads student file return null for error
//all memeory allocations should be freed in destroy unless given null
Node* createStudentListFromFile(FILE* studentsFile){

    Node head=malloc(sizeof(Node));
    Node tail=malloc(sizeof(Node));
    if(!head||!tail){
        return NULL;//malloc fail
    }
    head=NULL;

    int totalCredits;
    double GPA;
    char ID[ID_LENGTH], FirstName[MAX_STRING_LEN], lastName[MAX_STRING_LEN], city[MAX_STRING_LEN], department[MAX_STRING_LEN];
    
    while(scanf(studentsFile,"%9s %d %lf %99s %99s %99s %99s", ID, &totalCredits, &GPA, FirstName, lastName, city, department)==7)
    {
        if (!studentsFile){
            return NULL; //BAD PARAM
        }
        Student currentStudent=(Student)malloc(sizeof(Student));
        
        if(!currentStudent){
            return NULL;//MALLOC FAIL
        }

        strcpy(currentStudent->m_studentID,ID);
        strcpy(currentStudent->m_city,city);
        strcpy(currentStudent->m_department,department);
        strcpy(currentStudent->m_firstName,FirstName);
        strcpy(currentStudent->m_lastName,lastName);
        currentStudent->m_GPA=GPA;
        currentStudent->m_totalCredits=totalCredits;
        currentStudent->m_friends=NULL;
        currentStudent->m_enemies=NULL;
        

        Node nextNode=malloc(sizeof(Node));
        if(!nextNode){
            return NULL;//MALLOC FAIL
        }
        nextNode->m_item=currentStudent;
        nextNode->m_next=NULL;

        if (head==NULL){
            head=nextNode;
        }
        else{
            tail->m_next=nextNode;
        }
        tail=nextNode;
    }

    return head;


}


Node* createSCoursesListFromFile(FILE* coursesFile){
    if (!coursesFile){
        return NULL; //BAD PARAM
    }
    Node head=malloc(sizeof(Node));
    Node tail=malloc(sizeof(Node));
    if(!head||!tail){
        return NULL;//malloc fail
    }
    head=NULL;

    int courseNumber, courseSize;
    while(fscanf(coursesFile,"%d %d",&courseNumber, &courseSize)==2){
        Course newCourse=malloc(sizeof(Course));
        if(!newCourse){
            return NULL;//MALLOC FAIL
        }
        newCourse->m_number=courseNumber;
        newCourse->m_size=courseSize;

        Node newNode=malloc(sizeof(Node));
        if(!newNode){
            return NULL;//MALLOC FAIL
        }

        if (head==NULL){
            head=newNode;
        }
        else{
            tail->m_next=newNode;
        }
        tail=newNode;
    }
}



//returns file length and doesnt change the file ptr
int fileLength(FILE* file){
    fseek(file,0,SEEK_END);
    int length=ftell(file);
    fseek(file, 0, SEEK_SET);
    return length;

}









//=================================================================================
//FRIENDSHIP FUNCTIONS: they really explain themselves...
//==================================================================================


//TODO: rewrite this whole thing checks if 
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


