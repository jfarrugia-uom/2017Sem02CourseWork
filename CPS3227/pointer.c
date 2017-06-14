#include <stdio.h>

int main () {

   int  var = 20;   /* actual variable declaration */
   int  *ip;        /* pointer variable declaration */

   ip = &var;  /* store address of var in pointer variable*/

   printf("Address of var variable: %x\n", &var  );

   /* address stored in pointer variable */
   printf("Address stored in ip variable, address of pointer ip: %x,%x\n", ip, &ip );

   /* access the value using the pointer */
   printf("Value of *ip variable: %d\n", *ip );
   
   /* My additional statements */
   int *a = NULL;
   int *b = NULL;
   /* I set integer pointer b to point to same address as ip */
   b = ip;
   printf("b should be equal to ip = %d\n", *b);
   printf("b address should be equal to ip address = %x\n", b);
   /* now I'm setting a to the address ip */
   a = ip;
   var = 10;
   printf("a = %d\n", *a);
   printf("b = %d\n", *b);
   /* if i set b to another variable b should change but a should remain intact */
   int var2 = 500;
   b = &var2;
   printf("a = %d, b = %d\n", *a, *b);
   printf("b address = %x\n", b);
   /* finally set value of b  */
   *b = 600;
   printf("b address = %x; b value = %d; var2 = %d\n", b, *b, var2);
   
   char charArray[10] = "hello";
   char *charPointer = charArray;

   printf("char Array location = %x,%x\n", charPointer, &charArray[0]);
//   charPointer = "bye";
   printf("char Array location = %x,%x\n", charPointer, &charArray[0]);

   return 0;
}
