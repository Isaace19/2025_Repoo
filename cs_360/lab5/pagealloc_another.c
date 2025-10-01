#include "pagealloc.h"
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>


// write your page allocator functions here

#define PAGE_SIZE 4096 // 4kb of page size
#define BITS_PER_PAGE 2
#define PAGES_PER_BYTE (8 / BITS_PER_PAGE)
#define PAGES_PER_PAGE 4 // 4 pages per byte since every page is 2 bits in the BK region


static char *virtual_pool = NULL; // set the pointer to the beginning of the memory pool
static size_t num_page = 0; 	  // pages created initially set to 0, gets updated inside the page_init function
static bool init_flag = false;	  // flag to check if the page allocator is initialized or not
unsigned char *bookkeep = NULL; 	  // pointer to the bookkeeping region of the memory pool

static void *mmap_base = NULL;    // pointer to the mmap base addr - start of mmap region
static size_t mmap_length = 0;     // length of the mmap region - used for munmap later	

// using the given bit manip functions, now have to adapt the alloc code to handle 2 bit pages instead of just 1. or modify code here in order to accept 2 bit pages.
/*bool test_bit (char bitset, int index) {
    return (bitset >> index) & 1;
}

void set_bit (char *bitset, int index) {
    *bitset |= 1 << index;
}

void clear_bit (char *bitset, int index) {
    *bitset &= ~(1 << index);
}*/

// which = 0 -> taken, which = 1 -> last
bool test_bit(unsigned char *bitset, size_t page_index, int which) {
    size_t byte     = page_index / 4;                    // 4 pages per byte
    int bit_index   = 7 - 2 * (page_index % 4) - which;  // map page→bit
    return (bitset[byte] >> bit_index) & 1;
}

void set_bit(unsigned char *bitset, size_t page_index, int which) {
    size_t byte   = page_index / 4;
    int bit_index = 7 - 2 * (page_index % 4) - which;
    bitset[byte] |= (1 << bit_index);
}

void clear_bit(unsigned char *bitset, size_t page_index, int which) {
    size_t byte   = page_index / 4;
    int bit_index = 7 - 2 * (page_index % 4) - which;
    bitset[byte] &= ~(1 << bit_index);
}

// This function should be in charge of creating the memory pool with BK space for however many pages get initialized in the program.
// every page group is 4kB from this contigous initialized pool
bool page_init(size_t pages) {
	// set up the page pool - this is the contingous region of memory carved via mmap. This serves as a heap of pages. 
	if (init_flag) {
		return false;
	}

	if (pages < 2 || pages > (1 << 18)) {
		printf("Page limits error out.\n");
		return false;
	}

	// TODO: Check if page size is calculated properly, work with Bookkeeping pages in the init function here. 
	// One byte of BK is responsible for four pages, therefore: 1 BK is created for every 4096 * 4 pages in the mem pool created.

	num_page = pages;
	// carve out the BK_PAGES worth of mem from the mmap addr that we created above. 
	size_t BK_BYTES = ((pages * 2 + 7) / 8);
	
	// rounding for bookkeeping pages calcuation
	size_t BK_PAGES = (BK_BYTES / PAGE_SIZE) + (BK_BYTES % PAGE_SIZE != 0 ? 1 : 0);

	size_t length = (BK_PAGES + pages) * PAGE_SIZE;
	// map enough memory for bookkeeping pages using mmap
	void *addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE |
					MAP_ANONYMOUS, -1, 0);

	if (addr == MAP_FAILED) {
		return false;
	}
	bookkeep = (unsigned char*)addr; // set the BK pointer to the beginning of the mmap memory region.
	// set the virtual pool pointer to the start of usable memory after BK has been set aside first.
	virtual_pool = (char *)addr + BK_PAGES * PAGE_SIZE;

	// zero out the bookkeeping region to keep everything aligned properly
	memset(bookkeep, 0, BK_PAGES * PAGE_SIZE);

	init_flag = true;
	mmap_base = addr;
	mmap_length = length;
	return true;
}

void page_deinit(void) {
	if (init_flag) {
		// if true, work to unitialize everything and set the global variables back to original/0
		munmap(mmap_base, mmap_length);
		num_page = 0;
		virtual_pool = NULL;
		init_flag = false;

		bookkeep = NULL;
	}
}

// Keep track of the pages taken and which are free is the function of page_alloc
void *page_alloc(size_t pages) {
	
	// Revised Algorithm:
	// 1. Search through the BK bytes from the first non-taken 0 bit page. Mark that as our starting location
	// 2. Check next page to see if the taken bit is 0 
	// 3. if taken = 0, then go back and repeat step 2 size_t pages times
	// 4. however, if taken = 1, then go back to step 1 with the next page as the starting point.
	// 5. if pages of contigous free pages is found, mark all pages as taken, set bit to 1, and the last page bit must be set to 1 as well.
	// 6. Take the BK page as the start of the allocation, scale by 4096. Add this to te memory address of the very first 4kilobyte chunk
	// 7. test the value for alginment, if not a multiple of 4096, then the math alignment is wrong.
	// return the memory address as the page. 
	
	if (pages == 0) {
		return NULL;
	}	
	if (!init_flag || pages > num_page) {
		return NULL; // pages don't add up or there was a problem with creating the memory pool.
	}

	// scan through the BK region and find the free pages, loop through them and check the taken bits
	// loop through and search:
	for (size_t i = 0; i <= num_page - pages; i++) {
		bool found = true;
        size_t taken_pages_index = 0;
		for(size_t j = 0; j< pages; j++) {
			// do the first loop to check if the pages are taken with a 1 bit
			if (test_bit(bookkeep, i + j, 0)) {
				found = false;
                taken_pages_index = i + j;
				break; // searches next pages if these bits are already taken
			}
		}
		if (found) {
			for(size_t j = 0; j < pages; j++) {
				set_bit(bookkeep, i + j, 0); // marks the page as taken

                // check to see if we actually set bits in this found code.
                if (j == pages - 1) {
                    set_bit(bookkeep, i + j, 1); // set last for last page
                } else {
                    clear_bit(bookkeep, i + j, 1); // clear last for others
                }
			}
			return virtual_pool + i * PAGE_SIZE; // return the addr of the first page.
		}else{
            i = taken_pages_index; // skip ahead to the next page after the taken one.
        }
		// work on setting the last bit for the last page allocated.
		// mark with the last bit
		/*size_t last_page_index = i + pages - 1;
		i = last_page_index;
		return virtual_pool + i * PAGE_SIZE; // return the addr of the first page.*/
	}	
	// if the loop exits, then we found no pages, so we just return out NULL. 
	return NULL;
}

void page_free(void *addr) {
	// free the block of pages specified by out pointer addr which should be returned from the page alloc
	if (!init_flag || addr == NULL) {
		return;
	}
	// 1. subtract mem address of the first page cunk given from the mem addr of the page
	// 2. divide that value by 4096 gives the page index since its 4kb per page
	// 3. two bookkeeping bits per page means that we multiply the index by 2 to get the proper index of the BK
	// 4. iterate through this and set all taken bits to 0. look for the last bit to be 1. set it to 0.

	size_t page_index = (char *)addr - virtual_pool;
	if (page_index % PAGE_SIZE != 0) {
		return; // bad alignment
	}
	page_index = page_index / PAGE_SIZE;
	if (page_index >= num_page) {
		return;
	}
	for(size_t i = page_index; i < num_page; i++) {
        if (!test_bit(bookkeep, i, 0)) {
            break;
        }
        clear_bit(bookkeep, i, 0);
        if (test_bit(bookkeep, i, 0)) {
            clear_bit(bookkeep, i, 1);
            break;
        }
	}
	
}


size_t pages_taken(void){
	size_t taken = 0;
	for(size_t i = 0; i < num_page; i++) {
		if (test_bit(bookkeep, i, 0)) {
			taken++;
		}
	}
	return taken;
}


size_t pages_free(void){
	// loop through the BK and count the amount of pages that are free, however do not use pages free in pages taken
	size_t free = 0;
	for (size_t i = 0; i < num_page; i++) {
		if (!test_bit(bookkeep, i, 0)) {
			free++;
		}
	}
	return free;
}
