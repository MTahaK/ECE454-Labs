#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "a3_malloc.h"

int m_init(void){
	void *start_addr = sbrk(0);
	int ret = brk(HEAP_SIZE + start_addr);
	void *end_addr = sbrk(0);
	printf("Start: %p. Increment: %d. End: %p\n", start_addr, HEAP_SIZE, end_addr);

	
	struct h_Node *new_node = (struct h_Node*) start_addr;
	new_node->c_blk = start_addr + sizeof(struct h_Node); // c_blk starts from data
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
			if( t->SIZE - size < closest && t->SIZE >= size + sizeof(struct h_Node)){
				
				closest = abs(t->SIZE - size);
				choice = t;
			}
		} 
		t = t->NEXT;
	}
	// Choice node found: if block size is larger than requested size, split
	if(choice != NULL){
		if(choice->SIZE > size){
			// Split. Essentially, restrict choice to requested size, set
			// NEXT of choice to be the address where the requested size
			// bounds it to.
			
			struct h_Node *new_node = (struct h_Node*) choice->c_blk + size;
			new_node->n_blk = choice->n_blk;
			choice->n_blk = choice->c_blk + size;
			new_node->c_blk = choice->n_blk + sizeof(struct h_Node);
			new_node->SIZE = choice->SIZE - (size + sizeof(struct h_Node));
			new_node->NEXT = choice->NEXT;
			new_node->STATUS = 0;
		
			choice->SIZE = size;
			choice->STATUS = 1;
			choice->NEXT = (struct h_Node*) choice->c_blk + size;
			return choice;
		} 
	}
}

void m_free(void *ptr){
	// Function will also perform coalescence through the list after the freed
	// block after freeing it.
	if(ptr != NULL){
		struct h_Node *block = (struct h_Node*) ptr;
		if(block->STATUS == 1){
			printf("Freeing node of size %lu\n", block->SIZE);
			block->STATUS = 0;
			// Coalescence
			while(block){
				if(block->NEXT != NULL && block->NEXT->STATUS == 0 && block->STATUS == 0){
					printf("Coalescing blocks of size %lu and %lu\n", block->SIZE, block->NEXT->SIZE);
					struct h_Node *next = block->NEXT;
					block->SIZE = block->SIZE + next->SIZE + sizeof(struct h_Node);
					block->n_blk = next->n_blk;
					next->c_blk = NULL;
					block->NEXT = next->NEXT;
					next = next->NEXT;
				}
				block = block->NEXT;
			}
			struct h_Node *t = h_list.list_head;
			while(t!=block){
				if(t->NEXT != NULL && t->NEXT->STATUS == 0 && t->STATUS == 0){
					printf("Coalescing blocks of size %lu and %lu\n", t->SIZE, t->NEXT->SIZE);
					struct h_Node *next = t->NEXT;
					t->SIZE = t->SIZE + next->SIZE + sizeof(struct h_Node);
					t->n_blk = next->n_blk;
					next->c_blk = NULL;
					t->NEXT = next->NEXT;
					next = next->NEXT;
				}
			t = t->NEXT;
			}
		}	
	}
}

void *m_realloc(void *ptr, size_t size){
	if(ptr != NULL){
		struct h_Node *block = (struct h_Node*) ptr;
		if(block->STATUS == 1){
			
			printf("Reallocating block starting at %p and of size %lu\n", block->c_blk, block->SIZE);
			struct h_Node *new_block = (struct h_Node*)m_malloc(size); 
			m_free(ptr);
			printf("Returning newly allocated block at %p of size %lu\n", new_block->c_blk, new_block->SIZE);
			return new_block;
		}
	}
	return NULL;
}

void h_layout(struct h_Node *ptr)
{
	int total_usage = 0;
	while (ptr != NULL)
	{
		printf("Block Address: %p, Size: %lu, True Size: %lu Status: %d\n", ptr->c_blk, ptr->SIZE, ptr->SIZE + sizeof(struct h_Node), ptr->STATUS);
		total_usage+=ptr->SIZE + sizeof(struct h_Node);
		ptr = ptr->NEXT;
	}
	printf("Total heap size: %d\n", total_usage);
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
	printf("================\n");
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node = m_malloc(10);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node1 = m_malloc(20);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node2 = m_malloc(30);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node3 = m_malloc(80);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node4 = m_malloc(90);
	h_layout(h_list.list_head);
	printf("================\n");
	m_free(node3);
	h_layout(h_list.list_head);
	printf("================\n");
	m_free(node4);
	h_layout(h_list.list_head);
	printf("================\n");
	struct h_Node *node_check = m_realloc(node1, (size_t)500);
	h_layout(h_list.list_head);
	printf("================\n");
	m_realloc(node, (size_t)70);
	h_layout(h_list.list_head);
	printf("================\n");
	m_realloc(node_check, (size_t)60);
	h_layout(h_list.list_head);
	printf("================\n");
	


	
	return 0;
}