#!/bin/bash

mkdir finalCheckTmpDir
unzip $1 -d finalCheckTmpDir 2> /dev/null
cd finalCheckTmpDir

gcc -std=c99 -o HackEnrollment -I/home/mtm/public/2223b/ex1/ -Itool -Wall -pedantic-errors -Werror -DNDEBUG *.c tool/*.c

if [ $? -ne 0 ] 
	then
		echo "Compilation failed"
		cd ..
		rm -r finalCheckTmpDir
		exit 0
fi

./HackEnrollment ../ExampleTest/students.txt ../ExampleTest/courses.txt ../ExampleTest/hackers.txt ../ExampleTest/queues.txt  res.txt

if [ $? -ne 0 ] 
	then
		echo "Program exited with non 0 exit code" 		
		cd ..
		rm -r finalCheckTmpDir
		exit 0
fi

diff res.txt ../ExampleTest/out.txt > /dev/null


if [ $? -ne 0 ] 
	then
		echo "Output of ExampleTest was incorrect"
		cd ..
		rm -r finalCheckTmpDir
		exit 0
fi

echo "Ready to submit :)"
cd ..
rm -r finalCheckTmpDir
