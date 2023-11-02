#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int HEAP_SIZE = 100000;
struct h_Node{
	int STATUS;				// 0: free, 1: allocated
	size_t SIZE;            // Size of block in bytes
	void *c_blk;            // Start address of current block
	void *n_blk;            // Start address of next block
	struct h_Node *NEXT;
};
/* * 
 * NOTES:
 * c_blk ALWAYS starts AFTER the overhead from the size of h_Node. That is, each block
 * of memory starts at the start address, and consists of 'data' starting from 
 * start_addr + sizeof(h_Node). 
 * 
 * n_blk is ALWAYS at c_blk + SIZE.
 * NEXT points to the start of the header of the next node. That is, it points to 
 * the start address of the next node, AKA at c_blk (of the next node) - sizeof(h_Node).
 * Therefore, the NEXT node starts right after n_blk, and the c_blk of the NEXT node
 * starts at n_blk + sizeof(h_Node).
 */
struct h_List{
	struct h_Node *list_head;
} h_list;

// Memory management functions:
int m_init(void);
void *m_malloc(size_t size);
void m_free(void *ptr);
void *m_realloc(void *ptr, size_t size);


// Heap list:
void h_layout(struct h_Node *ptr);
int m_check(void);
