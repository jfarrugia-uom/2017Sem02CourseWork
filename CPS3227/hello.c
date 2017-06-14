#include <stdio.h>
#include <stdlib.h>

int main() {
	/* my first program in C */
	printf("Hello, World! \n");
   
	char *c = "hello world James";
	printf("string: %s; address: %x\n", c, &c);
 	c = "hello world";
	printf("string: %s; address: %x\n", c, &c);
	
	/* TO ASK: slight confusion when allocating character pointers vs integer pointers*/
	int *int_pointer = NULL;
	char *char_pointer = NULL;
	int var = 20;
	int_pointer = &var;
	//char_pointer = "ten";
	printf("int = %d\n", *int_pointer);
	*int_pointer = 100;
	printf("pointer = %d, var = %d\n", *int_pointer, var);
	int *int_pointer2 = NULL;

	int_pointer2 = malloc(sizeof(int));
	*int_pointer2 = 500;
	printf("pointer2 = %d\n", *int_pointer2);
	free(int_pointer2);
	*int_pointer2 = 1;
	printf("pointer2 = %d\n", *int_pointer2);
	/* should get a Segmentation fault!*/
	printf("var size = %d\n", sizeof(var));
  
   return 0;
}
