# Hack Enrollment System

## Overview
**Hack Enrollment System** is a C-based implementation designed to solve the problem of efficient enrollment management using advanced data structures and algorithms. At its core, the project incorporates an Israeli Queue, a specialized data structure that extends a standard queue by prioritizing elements based on relationships like friendships and rivalries. The system is optimized for time complexity, ensuring fast insertion and efficient validation of constraints. Additionally, the project emphasizes robust memory management, adhering to C's low-level memory control principles, ensuring no memory leaks or invalid accesses while handling dynamic data.

---

## Features
- **Student and Course Management**:
  - Add and manage students with attributes like ID, GPA, name, and department.
  - Define courses with specific capacities.
- **Constraint Enforcement**:
  - Handle friendship and rivalry thresholds to ensure valid enrollments.
  - Adjust priorities dynamically based on relationships.
- **Error Detection**:
  - Identify and report issues in input files or constraints.
- **Memory Safety**:
  - Fully compatible with Valgrind, ensuring no memory leaks.

---

## File Structure
### Core Files
- **`HackEnrollment.h` / `HackEnrollment.c`**:
  - Implements enrollment logic and constraint validation.
- **Input Files**:
  - `students.txt`: Contains student details (ID, GPA, name, department).
  - `courses.txt`: Lists courses with their capacities.
  - `hackers.txt`: Specifies hacker constraints, including desired courses, friends, and rivals.
- **Output Files**:
  - Contains successful enrollments and detailed error messages.

---

## How to Build
1. Clone the repository:
   ```bash
   git clone <repository_url>
   cd hack-enrollment
   ```
2. Compile the program:
   ```bash
   gcc -std=c99 -o HackEnrollment -Wall -pedantic-errors -Werror -DNDEBUG *.c tool/*.c
   ```

---

## How to Run
To execute the program, provide the input and output file paths:
```bash
./HackEnrollment <students_file> <courses_file> <hackers_file> <output_file>
```

### Example Command
```bash
./HackEnrollment students.txt courses.txt hackers.txt output.txt
```

---

## Testing
1. Run the program with provided test files:
   ```bash
   ./HackEnrollment students.txt courses.txt hackers.txt output.txt
   ```
2. Verify memory safety with Valgrind:
   ```bash
   valgrind --leak-check=full ./HackEnrollment students.txt courses.txt hackers.txt output.txt
   ```

---

## Error Handling
The program handles various errors, including:
- **Input Errors**:
  - Missing or invalid files.
  - Incorrect file formatting.
- **Enrollment Constraints**:
  - Unmet rivalry or friendship thresholds.
  - Exceeding course capacities.
- **Output**:
  - Generates clear and structured error messages.

---

## Design Principles
- **Modular Code**:
  - Ensures maintainability and reusability.
- **Memory Management**:
  - Prevents leaks and ensures stability.
- **Scalability**:
  - Handles large datasets efficiently.

---

## Example Input and Output
### Input Files
#### `students.txt`
```
123456,3.5,JohnDoe,CS
654321,4.0,JaneSmith,Math
```
#### `courses.txt`
```
Algorithms,2
DataStructures,3
```
#### `hackers.txt`
```
123456,Algorithms,JaneSmith,-
654321,DataStructures,JohnDoe,-
```

### Output File (`output.txt`)
```
JohnDoe enrolled in Algorithms
JaneSmith enrolled in DataStructures
```
