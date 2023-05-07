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
 * in case of failure returns null-memory handled in function
 *
 * */
EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers);

/**Reads Enrollment queues and updates the relevant m_coursesList item accordingly
 * @param queues: <course number> <ID>...<ID>\n
 *
 * CAUTION: in case of error returns NULL, USER  is responsible to BACK UP original system pointer!!!
 *
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues);


/**Updates all queues to IsraeliQueues
 * assumes sys and file parameters are all correct!
 *
 * in cade of error prints out "help!!"
 * */
void hackEnrollment(EnrollmentSystem sys, FILE* out);




/**
 * switches to lower case
 */

void ignoreUpper(EnrollmentSystem sys);



/**
 * destroyEnrollment
 */
 void destroyEnrollment(EnrollmentSystem sys);


/**
 * returns 1 if equal and 0 if not
 */
int isTheSameStudent(void* stuA, void* stuB);


//TODO this shouldn't be in the final version
void TESTFUNCTION(EnrollmentSystem sys);
void printQueuesToFile(EnrollmentSystem sys, FILE* outFile);


#endif