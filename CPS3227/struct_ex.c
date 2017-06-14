#include <stdio.h>
#include <stdlib.h>

struct _Student {
	int id;
	char *name;
};

typedef struct _Student Student;

Student *createStudent (int id, char *name);

int main() {
	Student *s = createStudent(1, "James");
	int var = s->id;
	printf("Id = %d\n", s->id);
	return 0;
}

Student *createStudent (int _id, char *_name) {
	Student *student = malloc(sizeof(Student));
	student->id = _id;
	student->name = _name; 
	return student;
}
