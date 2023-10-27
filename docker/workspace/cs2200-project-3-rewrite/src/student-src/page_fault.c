#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   pfn_t pfn = free_frame();
   vpn_t vpn = vaddr_vpn(addr);
   pte_t* page_table_entry = (pte_t*) (mem + (PTBR * PAGE_SIZE)) + vpn;
   uint8_t* location = (mem + (pfn * PAGE_SIZE));
   if(swap_exists(page_table_entry)){
      swap_read(page_table_entry, location);
   } else {
      memset(location, 0, PAGE_SIZE);
   }   

   page_table_entry->pfn = pfn;
   page_table_entry->valid = 1;
   page_table_entry->dirty = 0;
   
   fte_t* frame_table_entry = frame_table + pfn;
   frame_table_entry->referenced = 1;
   frame_table_entry->protected = 0;
   frame_table_entry->process = current_process;
   frame_table_entry->mapped = 1;
   frame_table_entry->vpn = vpn;

   stats.page_faults++;
}

#pragma GCC diagnostic pop
