#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct h_Node{
	int STATUS;
	size_t SIZE;
	void *c_blk;
	void *n_blk;
	struct h_Node *NEXT;
};

struct h_List{
	struct h_Node *list_head;
};

// Memory management functions:
int m_init(void);
void *m_malloc(size_t size);
void m_free(void *ptr);
void *m_realloc(void *ptr, size_t size);


// Heap list:
struct h_Node *create_h_node(int status, size_t size, void *c_blk);
void h_layout(struct h_Node *ptr);
