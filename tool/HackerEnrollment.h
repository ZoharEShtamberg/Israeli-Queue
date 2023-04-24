//created 19/4/23 by Daniella Shimon
#ifndef PROVIDED_HACKERENROLLMENT_H
#define PROVIDED_HACKERENROLLMENT_H

#include <stdio.h>
#include <stdbool.h>
#include "IsraeliQueue.h"


typedef struct EnrollmentSystem_t *EnrollmentSystem;
typedef struct Student_t *Student;
typedef struct Hacker_t *Hacker;
typedef struct Course_t *Course;


typedef enum { HACKER_ENROLLMENT_SUCCESS, HACKER_ENROLLMENT_ALLOC_FAILED, HACKER_ENROLLMENT_BAD_PARAM, HACKER_ENROLLMENT_ERROR } HackerEnrollmentError;
/**Error clarification:
 * HACKER_ENROLLMENT_SUCCESS: Indicates the function has completed its task successfully with no errors.
 * HACKER_ENROLLMENT_ALLOC_FAILED: Indicates memory allocation failed during the execution of the function.
 * HACKER_ENROLLMENT_BAD_PARAM: Indicates an illegal parameter was passed.
 * HACKER_ENROLLMENT_ERROR: Indicates any error beyond the above.
 * */

/**Creates a new Enrollment system based on the files provided, files should be
 * according to to following structure:
 * @param students: <Student ID> <Total Credits> <GPA> <Name> <Surname> <City> <Department>\n
 * @param courses: <Course Number> <size>\n
 * @param hackers: <Student ID>\n <Course Number>\n <Student ID>\n (friends) <Student ID>\n (rivals)
 * 
 * 
 * returns a pointer
 * */
EnrollmentSystem createEnrollment(FILE* students, FILE* courses, FILE* hackers);

/**Function Description:
 * 
 * */
EnrollmentSystem readEnrollment(EnrollmentSystem sys, FILE* queues);


/**Function Description:
 * 
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

#endif