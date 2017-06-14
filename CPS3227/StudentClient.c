#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"  /* Include the header here, to obtain the function declaration */



// define storage mechanism outside student.c
void storeStudentInTextFile(Student *student);
void loadStudentsFromTextFile(StudentCollection *header);

void test1();

void test1() {
	// Initialise StudentCollection
	StudentCollection *coll = createCollection(); 
    Student *s = createStudent ( 1,  37, "James Farrugia", 30);		
    // push student to end of list
    pushbackStudent(coll, s);
	displayAllStudents(coll);
	
	// delete student at head
	deleteStudentAt(coll, 0);
	displayAllStudents(coll);
	
	// let's create a new student and append to end of list	
	Student *s2 = createStudent ( 2,  33, "Paul Said", 30);
	pushbackStudent(coll, s2);	
	displayAllStudents(coll);
	
	/* new student and insert between James and Paul, i.e. at position 1 */
	Student *s3 = createStudent(3, 7, "Julian Borg", 12);
	insertStudentAt(coll, s3, 1);
	displayAllStudents(coll);
	/* add final student */
	Student *s4 = createStudent(4, 21, "Mark Axisa", 23);
	insertStudentAt(coll, s4, 0);
	displayAllStudents(coll);

	/* modify sutdent at position 4 */
	modifyStudentAt(coll, 0, 4, 22, "Mark S. Axisa");
	displayAllStudents(coll);
}

int main(void)
{			
	StudentCollection *coll = createCollection(); 
    Student *s = createStudent ( 1,  37, "James Farrugia", 30);		
    // push student to end of list
    pushbackStudent(coll, s);
	displayAllStudents(coll);		

	// set storage/loading strategies
	storeMechanism storeStrat = &storeStudentInTextFile;
	loadMechanism loadStrat = &loadStudentsFromTextFile;

//	printf("%s\n", "I'm storing my student collection in a file <student.lst>");	
//	storeCollection(storeStrat, coll);
	
	loadStrat(coll);
	displayAllStudents(coll);
 	
    return 0;
}

void storeStudentInTextFile(Student *student) {

	FILE *file;
	file = fopen("student.lst", "a+");
	fputs(toCommaString(student), file);
	fclose(file);
}

void loadStudentsFromTextFile(StudentCollection *header) {
	printf("%s\n", "I'm loading my students from a file");	

	// destroy collection
	free(header);
	header = createCollection(); 
	char line[255];
	//char *token, *string, *tofree;
//	char *tokens[100];
	char *tokens[100];
	FILE *file;
	file = fopen("student.lst", "a+");
	
	while (fgets(line, 255, file)) {	
		int i = 0;
		/*tofree = string = strdup(line);
		while ((token = strsep(&string, ",")) != NULL) {
			printf("Adding: %s", token);						
		}
		free(tofree);*/
		tokens[i] = strtok(line,",");
		while(tokens[i] != NULL) {
			tokens[++i] = strtok(NULL,",");
		}
		char *temp = (char *)malloc(strlen(tokens[1])+1);
		strcpy(temp, tokens[1]);

		printf("test = %s,%s\n", tokens[1], *(&tokens[1]));
		Student *s = createStudent(atoi(tokens[0]), atoi(tokens[2]), temp, 0);

		pushbackStudent(header, s);
	}

	fclose(file);
}

