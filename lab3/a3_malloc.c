#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "a3_malloc.h"

struct h_Node *create_h_node(int status, size_t size, void *c_blk){
	struct h_Node *new_node = (struct h_Node*)m_malloc(sizeof(struct h_Node));
	new_node->STATUS = status;
	new_node->SIZE = size;
	new_node->c_blk = c_blk;
	new_node->n_blk = NULL;

	return new_node;
}

void h_layout(struct h_Node *ptr){
	while(ptr) {
        printf("Block Address: %d, Size: %d, Status: %d\n", ptr->c_blk, ptr->SIZE, ptr->STATUS);
        ptr = ptr->NEXT;
    }
}

struct h_List *init_empty_list(){
	struct h_List h_List;
	h_List.list_head = NULL;
	return &h_List;
}