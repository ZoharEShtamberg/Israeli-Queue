//
// Created by Zohar Shtamberg on 24/04/2023.
//
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "HackerEnrollment.h"

int main(int argc, char** argv){
	if(argc<6 || argc>7){
		return -1;//incorrect usage!
	}
	bool ignoreUpperFlag = (strcmp(argv[1], "-i")) ? false: true; //inverse of strcmp
	int i=(ignoreUpperFlag) ? 2 : 1;
	FILE* students = fopen(argv[i++], "r");
	FILE* courses = fopen(argv[i++], "r");
	FILE* hackers = fopen(argv[i++], "r");
	FILE* queues = fopen(argv[i++], "r");
	FILE* target = fopen(argv[i++], "w");
	if(!students || !courses || !hackers || !queues || !target){
		students && fclose(students);
		courses && fclose(courses);
		hackers && fclose(hackers);
		queues && fclose(queues);
		target && fclose(target);
		return -1;
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
