#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "a3_malloc.h"

// Coalescence
// if(t->NEXT != NULL && t->NEXT->STATUS == 0){
// 				struct h_Node *next = t->NEXT;
// 				t->SIZE = t->SIZE + next->SIZE;
// 				t->n_blk = next->n_blk;
// 				next->c_blk = NULL;
// 				t->NEXT = next->NEXT;
// 				next = next->NEXT;
// 				continue;
// 			}
int m_init(void){
	void *start_addr = sbrk(0);
	int ret = brk(HEAP_SIZE + start_addr);
	void *end_addr = sbrk(0);
	printf("Start: %p. Increment: %d. End: %p\n", start_addr, HEAP_SIZE, end_addr);

	
	struct h_Node *new_node = (struct h_Node*) start_addr;
	new_node->c_blk = start_addr + sizeof(struct h_Node);
	new_node->STATUS = 0;
	new_node->SIZE = HEAP_SIZE - sizeof(struct h_Node);
	new_node->n_blk = new_node->c_blk + new_node->SIZE;
	new_node->NEXT = NULL;
	h_list.list_head = new_node;

	return ret;
}
void *m_malloc(size_t size){
	// Search list from start, choose block with closest size to requested size
	struct h_Node *t = h_list.list_head;
	int closest = INT_MAX;
	struct h_Node *choice = NULL;
	while(t!=NULL){
		// printf("Status: %d, Size: %lu\n", t->STATUS, t->SIZE);
		if(t->STATUS == 0){
			if(t->SIZE == size){
				choice = t;
				choice->STATUS = 1;
				return choice;
			} // Block with exact size not found, keep looking
			if( t->SIZE - size < closest && t->SIZE > size){
				
				closest = abs(t->SIZE - size);
				choice = t;
			}
		} 
		t = t->NEXT;
	}
	// Choice node found: if block size is larger than requested size, split
	if(choice != NULL){
		if(choice->SIZE > size){
			// printf("Choice Status: %d, Size: %lu, Next: %p, Start: %p, End: %p\n", choice->STATUS, choice->SIZE, choice->NEXT, choice->c_blk, choice->n_blk);
			// Split. Essentially, restrict choice to requested size, set
			// NEXT of choice to be the address where the requested size
			// bounds it to.
			
			struct h_Node *new_node = (struct h_Node*) choice->c_blk + size + sizeof(struct h_Node);
			new_node->n_blk = choice->n_blk;
			choice->n_blk = choice->c_blk + size;
			new_node->c_blk = choice->n_blk + 1;
			new_node->SIZE = choice->SIZE - (size + sizeof(struct h_Node));
			new_node->NEXT = choice->NEXT;
			new_node->STATUS = 0;
		
			choice->SIZE = size;
			choice->STATUS = 1;
			choice->NEXT = (struct h_Node*) choice->c_blk + size + sizeof(struct h_Node);
		} 
	}
}

void m_free(void *ptr){
	// Function will also perform coalescence through the list after the freed
	// block after freeing it.
	if(ptr != NULL){
		struct h_Node *block = (struct h_Node*) ptr;
		if(block->STATUS == 1){
			block->STATUS = 0;
			// Coalescence
			while(block){
				if(block->NEXT != NULL && block->NEXT->STATUS == 0 && block->STATUS == 0){
					printf("Coalescing blocks of size %lu and %lu\n", block->SIZE, block->NEXT->SIZE);
					struct h_Node *next = block->NEXT;
					block->SIZE = block->SIZE + next->SIZE;
					block->n_blk = next->n_blk;
					next->c_blk = NULL;
					block->NEXT = next->NEXT;
					next = next->NEXT;
				}
				block = block->NEXT;
			}
		}
	}
}
void h_layout(struct h_Node *ptr)
{
	while (ptr != NULL)
	{
		printf("Block Address: %p, Size: %lu, Status: %d\n", ptr->c_blk, ptr->SIZE, ptr->STATUS);
		ptr = ptr->NEXT;
	}
}

int main(int argc, char *argv[])
{
	// void *c_break = sbrk(0);
	// printf("1 - %p \n", c_break);
	// c_break = sbrk(0);
	// printf("2 - %p \n", c_break);
	// c_break = sbrk(0);
	// printf("3 - %p \n", c_break);
	// c_break = sbrk(0);
	// printf("4 - %p \n", c_break);
	// c_break = sbrk(0);
	// printf("5 - %p \n", 5 + c_break);

	if(m_init() == 0){
		printf("Heap allocation successful. Allocated heap of %d.\n", HEAP_SIZE);
	} else{
		printf("Heap allocation unsuccessful.\n");
	}
	printf("\n================\n");
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node = m_malloc(50);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node1 = m_malloc(50);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node2 = m_malloc(60);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node3 = m_malloc(80);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node4 = m_malloc(50);
	h_layout(h_list.list_head);
	printf("================\n");
	printf("FREEING\n");
	m_free(node3);
	h_layout(h_list.list_head);
	printf("================\n");
	m_free(node2);
	h_layout(h_list.list_head);
	printf("================\n");
	m_free(node4);
	h_layout(h_list.list_head);
	printf("================\n");


	
	return 0;
}