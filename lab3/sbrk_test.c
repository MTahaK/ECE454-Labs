
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define Heap_Size 10000
char h_mem[Heap_Size] = {0};
void *h_mem_start;
void *h_mem_end;
void *c_break;

void *m_sbrk(int incr);

int main(void)
{
	
	struct h_Node
                  { 	
	 int       STATUS;
	 size_t SIZE;
	 void   *c_blk;
	 void   *n_blk;
	 struct h_Node * NEXT;
	}; 
	
	h_mem_start = (char *)h_mem;
	h_mem_end = (char *)h_mem + (Heap_Size -1);
	c_break = (void *)&h_mem;
	
	printf("\n h_mem= %p          c_break = %p", h_mem, c_break);
	printf("\n h_mem_start = %p   h_mem_end = %p", h_mem_start, h_mem_end);
	
	
	m_sbrk(0);
	printf("\n 1- h_mem_start=%p   c_break=%p   h_mem_end=%p", h_mem_start, c_break, h_mem_end);
	m_sbrk(+5);
                   printf("\n 2- h_mem_start=%p   c_break=%p   h_mem_end=%p", h_mem_start, c_break, h_mem_end);
	m_sbrk(+100);
                   printf("\n 3- h_mem_start=%p   c_break=%p   h_mem_end=%p", h_mem_start, c_break, h_mem_end);
	
	return (0);
}

void* m_sbrk(int incr)
{
    if(incr == 0)
    {
        return c_break;
    }
    if(incr < (char*)h_mem_start - (char*)c_break)
    {
        printf("Memory Underflow: No memory address prior to heap_start");
        return (void*) -1;
    }
    if(incr > (char*)h_mem_end - (char*)c_break) 
    {
        printf("Memory Overflow: No memory address after heap_end");
        return (void*) -1;
    }
	
    c_break = (void*)((char*)c_break+incr);
    return c_break;
}

