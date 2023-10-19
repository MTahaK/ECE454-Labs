#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int HEAP_SIZE = 10000;

struct h_Node{
	int STATUS;				// 0: free, 1: allocated
	size_t SIZE;            // Size of block in bytes
	void *c_blk;            // Start address of current block
	void *n_blk;            // Start address of next block
	struct h_Node *NEXT;
};

struct h_List{
	struct h_Node *list_head;
} h_list;

// Memory management functions:
int m_init(void);
void *m_malloc(size_t size);
void m_free(void *ptr);
void *m_realloc(void *ptr, size_t size);


// Heap list:
struct h_Node *create_h_node(int status, size_t size, void *c_blk);
void h_layout(struct h_Node *ptr);
