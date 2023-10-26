#include <stdio.h>
#include <stdlib.h>

struct a_test1{
    short a;
    int x;
    short b;
} table1[10];
struct a_test1 *p1, *q1;


int main(void){
    
    printf("a_test1:\n");
    // a_test1:
    // a:
    printf("Short: %lu, Int: %lu, Short: %lu\n", sizeof(short), sizeof(int), sizeof(short));
    // b:
    printf("Size of table1[0]: %lu, Size of table1: %lu\n",sizeof(table1[0]), sizeof(table1));
    // c:
    
}