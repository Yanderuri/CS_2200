#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 3 --------------------------------------
 * Checkout PDF section 4 for this problem
 * 
 * This function gets called every time a new process is created.
 * You will need to allocate a frame for the process's page table using the
 * free_frame function. Then, you will need update both the frame table and
 * the process's PCB. 
 * 
 * @param proc pointer to process that is being initialized 
 * 
 * HINTS:
 *      - pcb_t: struct defined in pagesim.h that is a process's PCB.
 *      - You are not guaranteed that the memory returned by the free frame allocator
 *      is empty - an existing frame could have been evicted for our new page table.
 * ----------------------------------------------------------------------------------
 */
void proc_init(pcb_t *proc) {
    // TODO: initialize proc's page table.
    // trust that it finds a free_frame()
    // or evicted someone
    pfn_t page_table = free_frame();
    memset(mem + page_table * PAGE_SIZE, 0, PAGE_SIZE);
    fte_t* process_frame = frame_table + page_table;
    process_frame -> protected = 1;
    process_frame -> mapped = 1;
    process_frame -> process = proc;

    proc -> saved_ptbr = page_table;
}

/**
 * --------------------------------- PROBLEM 4 --------------------------------------
 * Checkout PDF section 5 for this problem
 * 
 * Switches the currently running process to the process referenced by the proc 
 * argument.
 * 
 * Every process has its own page table, as you allocated in proc_init. You will
 * need to tell the processor to use the new process's page table.
 * 
 * @param proc pointer to process to become the currently running process.
 * 
 * HINTS:
 *      - Look at the global variables defined in pagesim.h. You may be interested in
 *      the definition of pcb_t as well.
 * ----------------------------------------------------------------------------------
 */
void context_switch(pcb_t *proc) {
    // TODO: update any global vars and proc's PCB to match the context_switch.
    PTBR = proc -> saved_ptbr;
}

/**
 * --------------------------------- PROBLEM 8 --------------------------------------
 * Checkout PDF section 8 for this problem
 * 
 * When a process exits, you need to free any pages previously occupied by the
 * process.
 * 
 * HINTS:
 *      - If the process has swapped any pages to disk, you must call
 *      swap_free() using the page table entry pointer as a parameter.
 *      - If you free any protected pages, you must also clear their"protected" bits.
 * ----------------------------------------------------------------------------------
 */
void proc_cleanup(pcb_t *proc) {
    // TODO: Iterate the proc's page table and clean up each valid page
    pte_t * current;
    for (size_t i = 0; i < NUM_PAGES; i++) {
        current = (pte_t*) (mem + (proc -> saved_ptbr * PAGE_SIZE)) + i;
        if (swap_exists(current)){
            swap_free(current);
        }
        if (current -> valid == 1){
            frame_table[current -> pfn].mapped = 0;
            current -> valid = 0;
        }
    }
    current = NULL;
    fte_t * oldFrameEntry = frame_table + proc -> saved_ptbr;
    oldFrameEntry -> protected = 0;
    oldFrameEntry -> mapped = 0;
}

#pragma GCC diagnostic pop
