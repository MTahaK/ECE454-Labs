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

// The output of the code is as follows:
/* Start: 0x55d7b6599000. Increment: 10000. End: 0x55d7b659b710
Heap allocation successful. Allocated heap of 10000.
================
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b659b710, Size: 9960, True Size: 10000 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 40
Space Utilization: 40 / 10000 = 0.0040
================
Time: 0.000003
Total memory allocated after last call: 2080
Number of operations in last call: 24
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b659b710, Size: 7920, True Size: 7960 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 2080
Space Utilization: 2080 / 10000 = 0.2080
================
Time: 0.000004
Total memory allocated after last call: 2620
Number of operations in last call: 27
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599a14, Size: 500, True Size: 540 Status: 1
Block Address: 0x55d7b6599a3c, End Addresss: 0x55d7b659b710, Size: 7380, True Size: 7420 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 2620
Space Utilization: 2620 / 10000 = 0.2620
================
Time: 0.000002
Total memory allocated after last call: 2960
Number of operations in last call: 30
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599a14, Size: 500, True Size: 540 Status: 1
Block Address: 0x55d7b6599a3c, End Addresss: 0x55d7b6599b68, Size: 300, True Size: 340 Status: 1
Block Address: 0x55d7b6599b90, End Addresss: 0x55d7b659b710, Size: 7040, True Size: 7080 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 2960
Space Utilization: 2960 / 10000 = 0.2960
================
Freeing node of size 500
Time: 0.000004
Number of operations in last call: 20
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599a14, Size: 500, True Size: 540 Status: 0
Block Address: 0x55d7b6599a3c, End Addresss: 0x55d7b6599b68, Size: 300, True Size: 340 Status: 1
Block Address: 0x55d7b6599b90, End Addresss: 0x55d7b659b710, Size: 7040, True Size: 7080 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 2460
Space Utilization: 2460 / 10000 = 0.2460
================
Time: 0.000004
Total memory allocated after last call: 4000
Number of operations in last call: 35
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599a14, Size: 500, True Size: 540 Status: 0
Block Address: 0x55d7b6599a3c, End Addresss: 0x55d7b6599b68, Size: 300, True Size: 340 Status: 1
Block Address: 0x55d7b6599b90, End Addresss: 0x55d7b659a16c, Size: 1500, True Size: 1540 Status: 1
Block Address: 0x55d7b659a194, End Addresss: 0x55d7b659b710, Size: 5500, True Size: 5540 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 4000
Space Utilization: 4000 / 10000 = 0.4000
================
Reallocating block starting at 0x55d7b6599a3c and of size 300
Freeing node of size 300
Coalescing blocks of size 500 and 300
Returning newly allocated block at 0x55d7b659a194 of size 800
Time: 0.000056
Total memory allocated after last call: 4200
Number of operations in last call: 79
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599b68, Size: 840, True Size: 880 Status: 0
Block Address: 0x55d7b6599b90, End Addresss: 0x55d7b659a16c, Size: 1500, True Size: 1540 Status: 1
Block Address: 0x55d7b659a194, End Addresss: 0x55d7b659a4b4, Size: 800, True Size: 840 Status: 1
Block Address: 0x55d7b659a4dc, End Addresss: 0x55d7b659b710, Size: 4660, True Size: 4700 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 4200
Space Utilization: 4200 / 10000 = 0.4200
================
Allocation unsuccessful: Requested size too high.
Time: 0.000002
Number of operations in last call: 2
================
Block Address: 0x55d7b6599028, End Addresss: 0x55d7b65997f8, Size: 2000, True Size: 2040 Status: 1
Block Address: 0x55d7b6599820, End Addresss: 0x55d7b6599b68, Size: 840, True Size: 880 Status: 0
Block Address: 0x55d7b6599b90, End Addresss: 0x55d7b659a16c, Size: 1500, True Size: 1540 Status: 1
Block Address: 0x55d7b659a194, End Addresss: 0x55d7b659a4b4, Size: 800, True Size: 840 Status: 1
Block Address: 0x55d7b659a4dc, End Addresss: 0x55d7b659b710, Size: 4660, True Size: 4700 Status: 0

Memory consistency check: PASSED
Total heap size: 10000, Total memory usage: 4200
Space Utilization: 4200 / 10000 = 0.4200
================ 
*/