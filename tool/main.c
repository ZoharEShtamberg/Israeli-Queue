//
// Created by Zohar Shtamberg on 24/04/2023.
//
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "HackerEnrollment.h"

int main(int argc, char** argv){
	if(argc<6 || argc>7){
		return 0;//incorrect usage!
	}
	bool ignoreUpperFlag = (strcmp(argv[1], "-i")) ? false: true; //inverse of strcmp
	int i=(ignoreUpperFlag) ? 2 : 1;
	FILE* students = fopen(argv[i++], "r");
	if(students==NULL){
		return 0;
	}
	FILE* courses = fopen(argv[i++], "r");
	if(courses==NULL){
		fclose(students);
		return 0;
	}
	FILE* hackers = fopen(argv[i++], "r");
	if(hackers==NULL){
		fclose(students);
		fclose(courses);
		return 0;
	}
	FILE* queues = fopen(argv[i++], "r");
	if(queues==NULL){
		fclose(students);
		fclose(courses);
		fclose(hackers);
		return 0;
	}
	FILE* target = fopen(argv[i++], "w");
	if(target==NULL){
		fclose(students);
		fclose(courses);
		fclose(hackers);
		fclose(queues);
		return 0;
	}
	EnrollmentSystem sys = createEnrollment(students, courses, hackers);
	if(ignoreUpperFlag){
		ignoreUpper(sys);
	}
	readEnrollment(sys, queues);
	hackEnrollment(sys, target);
	destroyEnrollment(sys);
	fclose(students);
	fclose(courses);
	fclose(hackers);
	fclose(queues);
	fclose(target);
	return 0;
}
