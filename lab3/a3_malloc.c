#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "a3_malloc.h"

// struct h_Node *create_h_node(int status, size_t size, void *c_blk)
// {
// 	struct h_Node *new_node = (struct h_Node *)m_malloc(sizeof(struct h_Node));
// 	new_node->STATUS = status;
// 	new_node->SIZE = size;
// 	new_node->c_blk = c_blk;
// 	new_node->n_blk = NULL;

// 	return new_node;
// }

// void h_layout(struct h_Node *ptr)
// {
// 	while (ptr)
// 	{
// 		printf("Block Address: %p, Size: %lu, Status: %d\n", ptr->c_blk, ptr->SIZE, ptr->STATUS);
// 		ptr = ptr->NEXT;
// 	}
// }

int main(int argc, char *argv[])
{
	void *c_break = sbrk(0);
	printf("1 - %p \n", c_break);
	c_break = sbrk(0);
	printf("2 - %p \n", c_break);
	c_break = sbrk(4);
	printf("3 - %p \n", c_break);
}