#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple Calculator */

/* function prototypes */
int add (int op1, int op2);
int sub (int op1, int op2);
/* client function prototype */
void simpleClient (int (*op)(int, int), void (*print)(int), int op1, int op2);

/* function implementation */
int add (int op1, int op2) {
	return op1 + op2;
}

int sub (int op1, int op2) {
	return op1 - op2;
}

void prettyPrint(int result) {
	printf("<<<<- This is our result ->>> %d\n", result);
}

void plainPrint(int result) {
	printf("Boring result = %d :(\n", result);
}

/* function pointer that accepts two integers and returns another integer */
typedef int (*CBFunctionOP)(int p_a, int p_b);
typedef void (*CBFunctionPrint)(int p_a);
/* typedef allows me to create custom types CBFunctionOP and CBFunctionPrint */

int main ( int argc, char *argv[] ) {
	int oper1, oper2 = 0;	
	
	// parse arguments and set to integers oper1, oper2

	CBFunctionOP myCalcFunc = NULL;
	CBFunctionPrint myPrintFunc = NULL;
	
	if (argc != 5) {
		printf("Usage: ./calc [operator add|sub] [print_style pretty|boring] [operand_1] [operand_2]\n");
	} else {
		sscanf (argv[3],"%d",&oper1);
		sscanf (argv[4],"%d",&oper2);		
		printf("argument count: %d\n", argc);	
						
		if ( strcmp(argv[1],"add") == 0 ) {
			myCalcFunc = &add;			
			
		} else if (strcmp(argv[1],"sub") == 0 ) {
			myCalcFunc = &sub;
			
		} else {	
			printf("Unknown operator %s; default to add", argv[1]);	
			myCalcFunc = &add;
		}	
				
		if ( strcmp(argv[2],"pretty") == 0 ) 
			myPrintFunc = &prettyPrint;	
		else 
			myPrintFunc = &plainPrint;

		simpleClient(myCalcFunc, myPrintFunc, oper1, oper2);
	} 
	
	return  0;
}

void simpleClient (int (*op)(int, int), void (*print)(int), int op1, int op2) {
	int result = 0;
	result = (*op)(op1, op2);
	(*print)(result);
}
