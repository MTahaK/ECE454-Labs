#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "a3_malloc.h"


// Memory usage of the entire heap. This therefore includes the headers of each block.
// Each header (AKA each h_Node) is 40 bytes. This is set to sizeof(h_Node) upon  
// initialization of the heap.
int mem_usage = 0;
/* * 
// NOTES:
// I have defined an operation to be one of the following:
// 
// Arithmetic Operations
// Assignment Operations
// Memory Accesses
// Conditional Checks
// Function Calls
// Loop Iterations
// Return Statements
// Pointer Operations
 */
int num_ops = 0;

/* * 
 * m_init:
 * Initializes heap. Takes no arguments, returns 0 for success, -1 for failure.
 * Initializes heap by creating an initial node pointed to by the h_list struct
 * declared in the header file. This initial node spans the entire requested heap size.
 * The SIZE of that node is HEAP_SIZE - 40, since 40 is the size of the h_Node struct
 * and the SIZE of each node counts the size of the block allocated after the header
 * portion of the block.
 */
int m_init(void){
	void *start_addr = sbrk(0);
	int ret = brk(HEAP_SIZE + start_addr);
	if(ret != 0) return -1;
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

/* * 
 * m_malloc:
 * Takes in requested amount of memory as argument, returns void pointer to the start
 * of the allocated block of memory (ie: at address c_blk - sizeof(h_Node)). 
 * First, the function checks through the list for any blocks that exactly match the
 * requested size. If so, it allocates that block and returns it. Otherwise, it 
 * continues searching through the list, and finds the block with the closest size to 
 * the requested size. Of note is how the function checks that such a block can support 
 * not only the requested size, but the size of an additional heap node too. This is 
 * because any such node, where the size is greater than the requested size, must be 
 * split into two nodes - one for the request, and whatever is left over. Thus, for the 
 * leftover memory, the free block must be large enough to support another h_Node 
 * header.
 * If such a node is found, as mentioned before, the node is then split into two 
 * h_Nodes, with the first being returned to the user, and the next node being left 
 * free.
 */

void *m_malloc(size_t size){
    
	// Search list from start, choose block with closest size to requested size
	if(size > HEAP_SIZE){
		printf("Allocation unsuccessful: Requested size too high.\n");
		num_ops += 2;
		return NULL;
	}
    
    if(size >= HEAP_SIZE - mem_usage){
        printf("Allocation unsuccessful: Requested size too high.\n");
		return NULL;
    }
	struct h_Node *t = h_list.list_head;
	int closest = INT_MAX;
	struct h_Node *choice = NULL;
	num_ops += 3;
	while(t!=NULL){
		num_ops += 1;
		// printf("Status: %d, Size: %lu\n", t->STATUS, t->SIZE);
		if(t->STATUS == 0){
			
			if(t->SIZE == size){
				choice = t;
				choice->STATUS = 1;
				// mem_usage += size + sizeof(struct h_Node);
				num_ops += 5;
				return choice;
			}
			num_ops += 2; // Block with exact size not found, keep looking
			if( t->SIZE - size < closest && t->SIZE >= size + sizeof(struct h_Node)){
				
				closest = abs(t->SIZE - size);
				choice = t;
				num_ops += 3;
			}
		}
        
		t = t->NEXT;
		num_ops += 2;
	}
	// Choice node found: if block size is larger than requested size, split
	if(choice != NULL){
        
		if(choice->SIZE > size){
			// Split. Essentially, restrict choice to requested size, set
			// NEXT of choice to be the address where the requested size
			// bounds it to.
			struct h_Node *new_node = (struct h_Node*) choice->c_blk + size;
            if(choice->n_blk == NULL) printf("here\n");
			new_node->n_blk = choice->n_blk;
			choice->n_blk = choice->c_blk + size;
			new_node->c_blk = choice->n_blk + sizeof(struct h_Node);
			new_node->SIZE = choice->SIZE - (size + sizeof(struct h_Node));
            
			new_node->NEXT = choice->NEXT;
			new_node->STATUS = 0;
		
            
			choice->SIZE = size;
			choice->STATUS = 1;
            
			choice->NEXT = (struct h_Node*) choice->c_blk + size;
			// mem_usage += size + sizeof(struct h_Node);
			num_ops += 13;
            
			return choice;
		} 
		num_ops += 1;
	}
	num_ops += 1;
	// Could not find block for size
	printf("Allocation unsuccessful: Requested size too high, or heap is full.\n");
}

/* * 
 * m_free:
 * Takes in a pointer to a block of memory as an argument. Returns nothing.
 * The function first deallocates the provided block of memory by setting its status
 * to 0 (free). After this, the function starts from the provided block of memory, and
 * works its way down the heap and coalesces any free nodes. Once this procedure has 
 * finished, the function starts back at the beginning of the list, and coalesces any 
 * free nodes down to the point of the provided block of memory. Therefore, this covers 
 * coalescence of the entire list.
 */
void m_free(void *ptr){
    // Function will also perform coalescence through the list after the freed
    // block after freeing it.
    num_ops += 1;  // For the if(ptr != NULL) check
    if(ptr != NULL){
        struct h_Node *block = (struct h_Node*) ptr;
        num_ops += 1;  // For the assignment and casting of ptr to block
        num_ops += 1;  // For the if(block->STATUS == 1) check
        if(block->STATUS == 1){
            printf("Freeing node of size %lu\n", block->SIZE);
            block->STATUS = 0;
            num_ops += 2;  // For the STATUS assignment and subtraction
            // mem_usage = mem_usage - block->SIZE;
            // Coalescence
            while(block){
                num_ops += 1;  // For the while(block) loop
                if(block->NEXT != NULL && block->NEXT->STATUS == 0 && block->STATUS == 0){
                    num_ops += 3;  // For the if condition checks
                    printf("Coalescing blocks of size %lu and %lu\n", block->SIZE, block->NEXT->SIZE);
                    struct h_Node *next = block->NEXT;
                    block->SIZE = block->SIZE + next->SIZE + sizeof(struct h_Node);
                    num_ops += 5;  // For the assignments and arithmetic operations
                    // mem_usage = mem_usage - next->SIZE - sizeof(struct h_Node);
                    block->n_blk = next->n_blk;
                    next->c_blk = NULL;
                    block->NEXT = next->NEXT;
                    next = next->NEXT;
                    num_ops += 5;  // For the assignments and pointer operations
                }
                block = block->NEXT;
                num_ops += 1;  // For block = block->NEXT assignment
            }
            struct h_Node *t = h_list.list_head;
            num_ops += 1;  // For the assignment of t
            while(t!=block){
                num_ops += 1;  // For the while(t!=block) loop
                if(t->NEXT != NULL && t->NEXT->STATUS == 0 && t->STATUS == 0){
                    num_ops += 3;  // For the if condition checks
                    printf("Coalescing blocks of size %lu and %lu\n", t->SIZE, t->NEXT->SIZE);
                    struct h_Node *next = t->NEXT;
                    t->SIZE = t->SIZE + next->SIZE + sizeof(struct h_Node);
                    num_ops += 5;  // For the assignments and arithmetic operations
                    // mem_usage = mem_usage - next->SIZE - sizeof(struct h_Node);
                    t->n_blk = next->n_blk;
                    next->c_blk = NULL;
                    t->NEXT = next->NEXT;
                    next = next->NEXT;
                    num_ops += 5;  // For the assignments and pointer operations
                }
                t = t->NEXT;
                num_ops += 1;  // For t = t->NEXT assignment
            }
        }   
    }
}

/* * 
 * m_realloc:
 * Takes in a pointer to a block of memory and the size to reallocate it as. Returns a
 * pointer to the reallocated node.
 * This function, after performing a few checks to ensure that reallocation of the
 * provided block of memory is possible (ie: checks if the block has been allocated),
 * simply calls m_malloc, followed by m_free, to reallocate a new block of memory for
 * the user. 
 */
void *m_realloc(void *ptr, size_t size){
    num_ops += 1;  // For the if(ptr != NULL) check
    if(ptr != NULL){
        struct h_Node *block = (struct h_Node*) ptr;
        num_ops += 1;  // For the assignment and casting of ptr to block
        
        num_ops += 1;  // For the if(block->STATUS == 1) check
        if(block->STATUS == 1){
            printf("Reallocating block starting at %p and of size %lu\n", block->c_blk, block->SIZE);
            struct h_Node *new_block = (struct h_Node*)m_malloc(size);
            num_ops += 1;  // For the new_block assignment (m_malloc call includes its own num_ops increments)
            
            m_free(ptr);  // m_free call includes its own num_ops increments
            
            printf("Returning newly allocated block at %p of size %lu\n", new_block->c_blk, new_block->SIZE);
            return new_block;
            num_ops += 1;  // For returning new_block
        }
    }
    num_ops += 1;  // For returning NULL
    return NULL;
}

/* * 
 * h_layout:
 * Takes in a pointer to a memory block. Returns nothing.
 * Traverses through heap list from the given location (usually the start of the list)
 * and prints out information on each block of memory. Also performs a memory
 * consistency check. At the end, it outputs information on the total heap size and
 * utilization.
 */
void h_layout(struct h_Node *ptr)
{
	int total = 0;
    mem_usage = 0;
	while (ptr != NULL)
	{
		printf("Block Address: %p, End Addresss: %p, Size: %lu, True Size: %lu Status: %d\n", ptr->c_blk, ptr->n_blk, ptr->SIZE, ptr->SIZE + sizeof(struct h_Node), ptr->STATUS);
		total +=ptr->SIZE + sizeof(struct h_Node);
        if(ptr->STATUS == 1) mem_usage += ptr->SIZE + sizeof(struct h_Node);
        else if(ptr->STATUS == 0) mem_usage += sizeof(struct h_Node);
		ptr = ptr->NEXT;
	}
	printf("\n");
	if(!m_check()) printf("Memory consistency check: PASSED\n");
	else printf("Memory consistency check: FAILED\n");
	printf("Total heap size: %d, Total memory usage: %d\n", total, mem_usage);
	printf("Space Utilization: %d / %d = %0.04f\n", mem_usage, total, (float)mem_usage/(float)total);
}

/* * 
 * m_check:
 * Takes in no arguments, and returns an int return code. A return of 0 is a pass, and
 * a return of -1 is a fail.
 * Traverses through heap and examines for any fragmentation in the heap structure.
 * Outputs pass if no fragmentation is detected.
 */
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

	// ! ALL TIMES POSTED HERE WERE GENERATED USING A UG MACHINE.
	
    if(m_init() == 0){
        printf("Heap allocation successful. Allocated heap of %d.\n", HEAP_SIZE);
    } else{
        printf("Heap allocation unsuccessful.\n");
    }
    printf("================\n");							// Space Utilization: 0.0040

    h_layout(h_list.list_head);
    printf("================\n");

    num_ops = 0;
    clock_t start_time = clock();
    char *pt1 = m_malloc(36585);								// Space Utilization: 0.2080, Num. Ops: 24, Time = 0.000005, Ops per sec: 4,800,000
    clock_t end_time = clock();

    printf("Time: %lf\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);	
    printf("Total memory allocated after last call: %d\n", mem_usage);
	printf("Number of operations in last call: %d\n", num_ops);

    h_layout(h_list.list_head);
    printf("================\n");

    num_ops = 0;
    start_time = clock();
    char *pt2 = m_malloc(500);								// Space Utilization: 0.2620, Num. Ops: 27, Time = 0.000005, Ops per sec: 5,400,000
    end_time = clock();
    
    printf("Time: %lf\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);								
    printf("Total memory allocated after last call: %d\n", mem_usage);
	printf("Number of operations in last call: %d\n", num_ops);

    h_layout(h_list.list_head);
    printf("================\n");

    num_ops = 0;
    start_time = clock();
    char *pt3 = m_malloc(300);								// Space Utilization: 0.2960, Num. Ops: 30, Time = 0.000005, Ops per sec: 6,000,000
    end_time = clock();
    
    printf("Time: %lf\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);
    printf("Total memory allocated after last call: %d\n", mem_usage);
	printf("Number of operations in last call: %d\n", num_ops);
    return 0;
}
