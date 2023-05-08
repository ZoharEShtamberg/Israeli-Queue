//created 19/4/23 by Daniella Shimon
#ifndef PROVIDED_HACKERENROLLMENT_H
#define PROVIDED_HACKERENROLLMENT_H

#include <stdio.h>
#include <stdbool.h>


typedef struct EnrollmentSystem_t *EnrollmentSystem;
typedef struct Student_t *Student;
typedef struct Hacker_t *Hacker;
typedef struct Course_t *Course;



/**Creates a new Enrollment system based on the files provided,
 * ALL files are expected to be formatted correctly!
 * 
 * FriendshipFunction array is initialized to null.
 * returns a pointer to the function-memory must be freed by destroyEnrollment
 * in case of failure returns null-memory allocations handled by callee
 *
 * */
EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers);


/**Reads Enrollment queues and updates the relevant m_coursesList item accordingly
 * @param queues: <course number> <ID>...<ID>\n
 * CAUTION: in case of error returns NULL, USER  is responsible to BACK UP original system pointer!!!
 * returns the pointer to the same Enrollment system upon successful read
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues);


/**Updates all queues to IsraeliQueues
 * assumes sys and file values are all correct!
 * if all hackers are satisfied prints new queues
 * if not prints first hacker on list that isn't
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out);



/**
 * switches names to lower case
 * @param sys must have filled in students list
 */
void ignoreUpper(EnrollmentSystem sys);



/**
 * destroysEnrollment-all allocated memory that isn't set to null is freed
 */
 void destroyEnrollment(EnrollmentSystem sys);


/**
 * returns 1 if equal and 0 if not
 * @param stuA,stuB must be pointers to valid students
 */
int isTheSameStudent(void* ptrA, void* ptrB);




#endif