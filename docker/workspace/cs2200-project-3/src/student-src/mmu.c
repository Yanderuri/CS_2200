#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    // Clear memory
    // First page belongs to frame table
    // One page worth of space?
    memset(mem, 0, PAGE_SIZE);
    // Init frame_table to start of memory
    frame_table = (fte_t *) mem;
    // set the first fte_t to be protected.
    frame_table -> protected = 1;
}

/**
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param access 'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to written to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entry in the process's page table are mapped in. 
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t addr, char access, uint8_t data) {
    // TODO: translate virtual address to physical, then perform the specified operation

    /* Either read or write the data to the physical address
       depending on 'rw' */

    // Get the VPN and offset from the virtual address
    vpn_t vpn = vaddr_vpn(addr);
    uint16_t offset = vaddr_offset(addr);
    // Get the page table entry from the process's page table
    pte_t *page_table = (pte_t *) (mem + PTBR * PAGE_SIZE + vpn);
    // Page fault if needed
    if (page_table -> valid == 0){
        page_fault(addr);
    }
    // PFN from VPN
    pfn_t pfn = page_table -> pfn;
    frame_table[pfn].referenced = 1;
    paddr_t phys_addr = (pfn << OFFSET_LEN) | offset;

    stats.accesses++;
    if (access == 'r') {
        return mem[phys_addr];
    } else {
        mem[phys_addr] = data;
        page_table -> dirty = 1;
        return 0;
    }
}