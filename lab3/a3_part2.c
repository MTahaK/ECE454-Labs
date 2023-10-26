#include <stdio.h>
#include <stdlib.h>

struct a_test1{
    short a;
    int x;
    short b;
} table1[10];
struct a_test1 *p1, *q1;

struct a_test2 {
    short a;
    short b;
    int x;
} table2[10];
 struct a_test2 *p2, *q2;

// After testing, I've found that a_test3 is already optimized.
struct a_test3 {
    char c;
    short s;
    int k;
    long l;
    float f;
    double d; 
} table3[10];
struct a_test3 *p3, *q3;

int main(void){
    
    printf("a_test1:\n");
    // a_test1:
    // a:
    printf("Short: %lu, Int: %lu, Short: %lu\n", sizeof(short), sizeof(int), sizeof(short));
    // b:
    printf("Size of table1[0]: %lu, Size of table1: %lu\n",sizeof(table1[0]), sizeof(table1));
    // c:
    printf("Alignment of table1[0]: %lu\n", _Alignof(table1[0]));  // _Alignof operator used to get alignment
    printf("Address of table1[0]: %p\n\n\n", (void*)&table1[0]);
    // d:
    for(int i = 1; i < 10; i++) {
        printf("Address of table1[%d]: %p\n", i-1, (void*)&table1[i-1]);
        printf("Address of table1[%d]: %p\n", i, (void*)&table1[i]);
        
        size_t difference = (size_t)((char*)&table1[i] - (char*)&table1[i-1]);
        printf("Difference in addresses (length of table1[%d]): %lu bytes\n", i-1, difference);
    }

    printf("\n\na_test2:\n");
    // a_test2:
    // a:
    printf("Short: %lu, Int: %lu, Short: %lu\n", sizeof(short), sizeof(short), sizeof(int));
    // b:
    printf("Size of table2[0]: %lu, Size of table2: %lu\n",sizeof(table2[0]), sizeof(table2));
    // c:
    printf("Alignment of table2[0]: %lu\n", _Alignof(table2[0]));  // _Alignof operator used to get alignment
    printf("Address of table2[0]: %p\n\n\n", (void*)&table2[0]);
    // d:
    for(int i = 1; i < 10; i++) {
        printf("Address of table2[%d]: %p\n", i-1, (void*)&table2[i-1]);
        printf("Address of table2[%d]: %p\n", i, (void*)&table2[i]);
        
        size_t difference = (size_t)((char*)&table2[i] - (char*)&table2[i-1]);
        printf("Difference in addresses (length of table2[%d]): %lu bytes\n", i-1, difference);
    }

    printf("\n\na_test3:\n");
    // a_test3:
    // a:
    printf("Char: %lu, Short: %lu, Int: %lu, Long: %lu, Float: %lu, Double: %lu\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(float), sizeof(double));
    // b:
    printf("Size of table3[0]: %lu, Size of table3: %lu\n",sizeof(table3[0]), sizeof(table3));
    // c:
    printf("Alignment of table3[0]: %lu\n", _Alignof(table3[0]));  // _Alignof operator used to get alignment
    printf("Address of table3[0]: %p\n\n\n", (void*)&table3[0]);
    // d:
    for(int i = 1; i < 10; i++) {
        printf("Address of table3[%d]: %p\n", i-1, (void*)&table3[i-1]);
        printf("Address of table3[%d]: %p\n", i, (void*)&table3[i]);
        
        size_t difference = (size_t)((char*)&table3[i] - (char*)&table3[i-1]);
        printf("Difference in addresses (length of table3[%d]): %lu bytes\n", i-1, difference);
    }

}