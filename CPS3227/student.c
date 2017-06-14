#include <stdlib.h>
#include <stdio.h>
#include "student.h"  /* Include the header (not strictly necessary here) */

/* using something called opaque definition of structure. I can only refer to the structure from within
   this implementation. Will not be able to access Student directly if not via this API */
   
struct _StudentCollection {
	Student *firstStudent;
	int nodeCount ;
};

struct _Student {
   int id;
   int age;
   char *name;
   int length;  
   Student *nextStudent; 
   Student *prevStudent;
} ;

Student *getStudentAtPosition(StudentCollection *header, int position);



StudentCollection *createCollection() {
	StudentCollection *coll = malloc(sizeof(StudentCollection));
	coll -> firstStudent = NULL;
	coll -> nodeCount = 0;
	return coll;
}

Student *createStudent (int _id, int _age, char *_name, int _length) {	
	Student *student = (Student*) malloc(sizeof(Student));
	student->id = _id;
	student->age = _age;
	student->name = _name; 
	student->length = _length;
	student->nextStudent = NULL;
	student->prevStudent = NULL;
	return student;
}


char* toString (Student *student) {
	char *buf;
	/* aware that is not ideal - should the record strings + extra text be longer than 256, the result will probably be truncated */
	buf = malloc(256);
	snprintf(buf, 256, "Id=%d; Name=%s; Age=%d", student->id, student->name, student->age);
	return buf;	
}

char* toCommaString(Student *student) {
	char *buf;
	/* aware that is not ideal - should the record strings + extra text be longer than 256, the result will probably be truncated */
	buf = malloc(256);
	snprintf(buf, 256, "%d,%s,%d\n", student->id, student->name, student->age);
	return buf;	
}

int getCollectionSize(StudentCollection *studentHeader) {
	return studentHeader -> nodeCount;
}

void pushbackStudent(StudentCollection *header, Student *student) {
	Student *currentStudent;
	
	/* check whether list is empty */
	if (header -> firstStudent == NULL) {
		header -> firstStudent = student;		
		student -> nextStudent = NULL;
		student -> prevStudent = NULL;
	} else { 
/*		currentStudent = header -> firstStudent;
		while (currentStudent -> nextStudent != NULL) {
			currentStudent = currentStudent -> nextStudent;			
		}*/
		// get student at position (nodeCount - 1) which is the last position
		currentStudent = getStudentAtPosition(header, (header -> nodeCount) - 1);		
		currentStudent -> nextStudent = student;
		student -> prevStudent = currentStudent;
	}	
	header -> nodeCount++;
}


void displayAllStudents(StudentCollection *studentHeader) {
	printf("---------------------------------------------\n");
	Student *currentStudent;
	currentStudent = studentHeader -> firstStudent;
	while (currentStudent != NULL) {
		printf("%s\n", toString(currentStudent));
		currentStudent = currentStudent -> nextStudent;
	}
	printf("-----|Collection size|-----> %d\n", studentHeader -> nodeCount);
}

/* "private" function to return student at position P */
Student *getStudentAtPosition(StudentCollection *header, int position) {
	Student *currentStudent = NULL;
	currentStudent = header -> firstStudent;
	if (currentStudent == NULL || ( position >= getCollectionSize(header) )) {
		return NULL;
	}
	
	int counter = 0;
	/* iterate over list until we find the node at desired position */
	while (counter < position) {
		currentStudent = currentStudent -> nextStudent;
		counter++;
	}
	return currentStudent;	
}

void insertStudentAt(StudentCollection *header, Student *newStudent, int position) {
	Student *currentStudent, *previousStudent;
	
	if (header -> firstStudent == NULL) {
		pushbackStudent(header, newStudent);
	}  else if (position >= getCollectionSize(header)) {
		/* silly position specified, simply add to end of list */
		pushbackStudent(header, newStudent);
	}  else {
		currentStudent = getStudentAtPosition(header, position);
		/* we know that a student will be returned because we checked for incorrect position earlier */	
		previousStudent = currentStudent -> prevStudent;
		if (previousStudent == NULL)  {
			// this means we're at the beginning of the list
			header -> firstStudent = newStudent;
			newStudent -> prevStudent = NULL;
		} else {
			previousStudent -> nextStudent = newStudent;
			newStudent -> prevStudent = previousStudent;
		}
		
		newStudent -> nextStudent = currentStudent;
		currentStudent -> prevStudent = newStudent;	
		header -> nodeCount++;														
	}			
}

void deleteStudentAt(StudentCollection *header, int position) {
	Student *currentStudent, *previousStudent;
	
	/* check that list is actually populated */
	if (header -> firstStudent == NULL) {
		return;
	}
	/* check that position is valid */
	if (position >= getCollectionSize(header)) {
		return;
	}
	
	currentStudent = getStudentAtPosition(header, position);
	previousStudent = currentStudent -> prevStudent;
					 		
	if (previousStudent == NULL) {
		// this means we're at the beginning of the list
		header -> firstStudent = currentStudent -> nextStudent;		
	} else {
		previousStudent -> nextStudent = currentStudent -> nextStudent;		
	}
	
	if (currentStudent -> nextStudent != NULL) {
		/* if null this means that we're trying to delete the last node which would not have a next node */
		currentStudent -> nextStudent -> prevStudent = previousStudent;
	}
	// finally free the memory occupied by the current student
	free(currentStudent);
	header -> nodeCount--;
	return;	
}

void modifyStudentAt(StudentCollection *header, int position, int id, int age, char *name) {
	Student *currentStudent;
	
	/* check that list is actually populated */
	if (header -> firstStudent == NULL) {
		return;
	}
	/* check that position is valid */
	if (position >= getCollectionSize(header)) {
		return;
	}
	
	currentStudent = getStudentAtPosition(header, position);
	currentStudent -> id = id;
	currentStudent -> age = age;
	currentStudent -> name = name;
	return;				
}


void storeCollection( storeMechanism store, StudentCollection *header) {
	Student *current = header -> firstStudent;
	while (current) {
		store(current);
		current = current -> nextStudent;
	}
}


