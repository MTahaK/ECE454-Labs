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
	mem_usage += sizeof(struct h_Node);

	return ret;
}

void *m_malloc(size_t size){
	// Search list from start, choose block with closest size to requested size
	if(size > HEAP_SIZE){
		printf("Allocation unsuccessful: Requested size too high.\n");
		return NULL;
	}
	struct h_Node *t = h_list.list_head;
	int closest = INT_MAX;
	struct h_Node *choice = NULL;
	while(t!=NULL){
		// printf("Status: %d, Size: %lu\n", t->STATUS, t->SIZE);
		if(t->STATUS == 0){
			if(t->SIZE == size){
				choice = t;
				choice->STATUS = 1;
				mem_usage += size + sizeof(struct h_Node);
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
			mem_usage += size + sizeof(struct h_Node);
			return choice;
		} 
	}
	// Could not find block for size
	printf("Allocation unsuccessful: Requested size too high, or heap is full.\n");
}

void m_free(void *ptr){
	// Function will also perform coalescence through the list after the freed
	// block after freeing it.
	if(ptr != NULL){
		struct h_Node *block = (struct h_Node*) ptr;
		if(block->STATUS == 1){
			printf("Freeing node of size %lu\n", block->SIZE);
			block->STATUS = 0;
			mem_usage = mem_usage - block->SIZE;
			// Coalescence
			while(block){
				if(block->NEXT != NULL && block->NEXT->STATUS == 0 && block->STATUS == 0){
					printf("Coalescing blocks of size %lu and %lu\n", block->SIZE, block->NEXT->SIZE);
					struct h_Node *next = block->NEXT;
					block->SIZE = block->SIZE + next->SIZE + sizeof(struct h_Node);
					mem_usage = mem_usage - next->SIZE - sizeof(struct h_Node);
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
					mem_usage = mem_usage - next->SIZE - sizeof(struct h_Node);
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
	int total = 0;
	while (ptr != NULL)
	{
		printf("Block Address: %p, Size: %lu, True Size: %lu Status: %d\n", ptr->c_blk, ptr->SIZE, ptr->SIZE + sizeof(struct h_Node), ptr->STATUS);
		total +=ptr->SIZE + sizeof(struct h_Node);
		ptr = ptr->NEXT;
	}
	printf("\n");
	if(!m_check()) printf("Memory consistency check: PASSED\n");
	else printf("Memory consistency check: FAILED\n");
	printf("Total heap size: %d, Total memory usage: %d\n", total, mem_usage);
	printf("Space Utilization: %d / %d = %0.04f\n", mem_usage, total, (float)mem_usage/(float)total);
}

int m_check(void){
	struct h_Node *t = h_list.list_head;
	while(t != NULL){
		if(t->NEXT != NULL && t->STATUS == 0 && t->NEXT->STATUS == t->STATUS){
			// Adjacent free blocks, fail consistency check
			return -1;
		}
		t = t->NEXT;
	}
	// Heap is consistent
	return 0;
}

int main(int argc, char *argv[])
{

	if(m_init() == 0){
		printf("Heap allocation successful. Allocated heap of %d.\n", HEAP_SIZE);
	} else{
		printf("Heap allocation unsuccessful.\n");
	}
	printf("================\n");							// Space Utilization: 0.0040

	h_layout(h_list.list_head);
	printf("================\n");
	char *pt1 = m_malloc(2000);								// Space Utilization: 0.2080
	printf("Memory allocated by last call: %d\n", mem_usage);
	h_layout(h_list.list_head);
	printf("================\n");
	char *pt2 = m_malloc(500);								// Space Utilization: 0.2620
	printf("Memory allocated by last call: %d\n", mem_usage);
	h_layout(h_list.list_head);
	printf("================\n");
	char *pt3 = m_malloc(300);								// Space Utilization: 0.2960
	printf("Memory allocated by last call: %d\n", mem_usage);
	h_layout(h_list.list_head);
	printf("================\n");
	m_free(pt2);											// Space Utilization: 0.2460
	h_layout(h_list.list_head);
	printf("================\n");
	char *pt4 = m_malloc(1500);								// Space Utilization: 0.4000
	printf("Memory allocated by last call: %d\n", mem_usage);
	h_layout(h_list.list_head);
	printf("================\n");
	char *pt5 = m_realloc(pt3, 800);						// Space Utilization: 0.4200
	printf("Memory allocated by last call: %d\n", mem_usage);
	h_layout(h_list.list_head);
	printf("================\n");
	m_malloc(10e9);											// Space Utilization: 0.4200
	printf("================\n");
	h_layout(h_list.list_head);
	printf("================\n");

	
	return 0;
}


// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node = m_malloc(10);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node1 = m_malloc(20);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node2 = m_malloc(30);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node3 = m_malloc(80);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node4 = m_malloc(90);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// m_free(node3);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// m_free(node4);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// struct h_Node *node_check = m_realloc(node1, (size_t)500);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// m_realloc(node, (size_t)70);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// m_realloc(node_check, (size_t)60);
	// h_layout(h_list.list_head);
	// printf("================\n");
	// printf("%d\n", m_check());