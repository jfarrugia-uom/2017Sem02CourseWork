#ifndef STUDENT_H   /* Include guard */
#define STUDENT_H

typedef struct _Student Student;

/* this will be our header node */
typedef struct _StudentCollection StudentCollection;

StudentCollection *createCollection();

/* returns pointer to student structure */
Student *createStudent (int id, int age, char *name, int length);
/* updates structure passed by reference */
void updateStudent (Student *student, int id, int age, char *name, int length);
char* toString (Student *student);
char* toCommaString(Student *student);
void pushbackStudent(StudentCollection *studentHeader, Student *nextStudent);
int getCollectionSize(StudentCollection *studentHeader);
void insertStudentAt(StudentCollection *studentHeader, Student *newStudent, int position);
void displayAllStudents(StudentCollection *studentHeader);
void printCSVStudentFields(Student *student);
void deleteStudentAt(StudentCollection *studentHeader, int position);
void modifyStudentAt(StudentCollection *studentHeader, int position, int id, int age, char *name);

/* rather than expect concrete implementations of storeStudent and listStudent let's use function pointers to allow
   several implementations - ex save to text file or some other format */

typedef void (*storeMechanism)(Student *student);
typedef void (*loadMechanism)(StudentCollection *header);


void storeCollection( storeMechanism, StudentCollection *header);


#endif // STUDENT_H

