//
// Created by sam on 24/04/18.
//

#include <exceptions.h>
#include <idt.h>
#include <paging.h>
#include <mem/mem.h>
#include <lib/string.h>
#include <log/log.h>
#include <mem/heap.h>
#include <lib/maths.h>
#include <lib/util.h>
#include <boot_defs.h>
#include <stdinc.h>
#include <arch.h>

#define HEAP_INDEX_SIZE 0x20000
// Frames per bitmap type
#define FRAMES_PER_BITMAP 32
#define FRAME_BITMAP_TYPE uint32_t
#define FRAME_FULL UINT32_MAX
#define FRAME_FOR(addr) (ARCH_ALIGN_DOWN(addr, ARCH_PAGE_SIZE) / PAGE_SIZE)
#define BITMAP_FOR_FRAME(frame) frame_bitmaps[frame / FRAMES_PER_BITMAP]
#define SET_FRAME(bitmap, offset) (bitmap |= (1 << offset))
#define CLEAR_FRAME(bitmap, offset) (bitmap &= ~(1 << offset))
#define BIT_OFFSET(frame) (frame % FRAMES_PER_BITMAP)
#define NUM_BITMAPS (num_frames_aligned / FRAMES_PER_BITMAP)
#define FRAME_IS_SET(frame) ((BITMAP_FOR_FRAME(frame) & (1 << BIT_OFFSET(frame))) != 0 ? true : false)

page_directory_t* kernel_directory = NULL;
heap_t* kernel_heap = NULL;
extern uint32_t pile_start, pile_end, pile_ptr;

uint32_t num_frames = 0, num_frames_aligned = 0;
FRAME_BITMAP_TYPE* frame_bitmaps = NULL;

void page_fault_handler(arch_cpu_state_t* registers) {
    EXCEPTION_PRINT_STATE(registers);
    uint32_t err_code = registers->err_code;
    bool protection_err = (bool)(err_code & PAGE_FAULT_PROTECTION), write_err = (bool)(err_code & PAGE_FAULT_WRITE),
        user_err = (bool)(err_code & PAGE_FAULT_USER), reserved_err = (bool)(err_code & PAGE_FAULT_RESERVED),
        exec_err = (bool)(err_code & PAGE_FAULT_EXEC);
    uint32_t bad_addr;
    asm ("mov %%cr2, %0": "=r"(bad_addr));
    PANIC("Page fault for virtual address 0x%x (%d): %s%s%s, %s, %s\n", bad_addr, bad_addr, reserved_err ? "reserved, " : "", exec_err ? "execution, " : "",
            protection_err ? "protection" : "non-present", write_err ? "write" : "read", user_err ? "user" : "supervisor");
}

// Mark a frame as allocated
void paging_set_frame(uint32_t frame) {
    if(frame < num_frames) SET_FRAME(BITMAP_FOR_FRAME(frame), BIT_OFFSET(frame));
}

// Mark continuous frames as allocated, from phys_start up to phys_end
void paging_set_frames(uint32_t phys_start, uint32_t phys_end) {
    phys_start = ARCH_ALIGN_DOWN(phys_start, ARCH_PAGE_SIZE);
    for (uint32_t frame = FRAME_FOR(phys_start); frame < FRAME_FOR(phys_end); ++frame) paging_set_frame(frame);
}

// Mark a frame as free
void paging_clear_frame(uint32_t frame) {
    if(frame < num_frames) CLEAR_FRAME(BITMAP_FOR_FRAME(frame), BIT_OFFSET(frame));
}

// Mark contiguous frames as free, from phys_start up to phys_end
void paging_clear_frames(uint32_t phys_start, uint32_t phys_end) {
    phys_start = ARCH_ALIGN_DOWN(phys_start, ARCH_PAGE_SIZE);
    for (uint32_t frame = FRAME_FOR(phys_start); frame < FRAME_FOR(phys_end); ++frame) paging_clear_frame(frame);
}

int paging_get_free_frame() {
    for (uint32_t i = 0; i < NUM_BITMAPS; ++i) {
        FRAME_BITMAP_TYPE bitmap = frame_bitmaps[i];
        // If at least one bit in the bitmap is clear
        if(bitmap != FRAME_FULL) {
            uint32_t frame = 0;
            // If it's zero then bit 1 is the first one free
            if(bitmap == 0) frame = 0;
            else {
                // The bit corresponding to the free page
                FRAME_BITMAP_TYPE mask = 0;
                mask = ~bitmap & (bitmap + 1);
                // Log in order to get the bit's position in the bitmap
                frame = log2_floor(mask);
            }
            return frame + i * FRAMES_PER_BITMAP;
        }
    }
    return -1;
}

physaddr_t paging_alloc_frame() {
    // Search for a free frame
    int free_frame = paging_get_free_frame();
    if(free_frame >= 0) {
        paging_set_frame((uint32_t) free_frame);
        return (physaddr_t) (free_frame * PAGE_SIZE);
    }
    return 0;
}

void paging_free_frame(physaddr_t addr) {
    uint32_t frame = FRAME_FOR(addr);
    if(!FRAME_IS_SET(frame)) PANIC("Attempting to free unallocated frame %d (0x%x)\n", frame, addr)
    else paging_clear_frame(frame);
}

void set_page_directory(page_directory_t* page_dir) {
    asm ("mov %0, %%cr3":: "r"((physaddr_t)page_dir));
}

void paging_init(uint32_t mem_kilobytes, uint32_t virtual_start, uint32_t virtual_end, uint32_t phys_start, uint32_t phys_end, uint32_t initrd_end) {
    // Align the page size to PAGE_SIZE
    // so we don't make a page table that oversteps the total memory
    uint32_t mem_aligned = ARCH_ALIGN_UP(mem_kilobytes * 1024, ARCH_PAGE_SIZE);

    /*
     * Memory map:
     *
     *                  Physical                                        Virtual
     *  0x000000        Grub                   -->  0x3221225472 (3GiB)         Grub
     *  0x100000 (1MiB) Kernel code start      -->  0x3222274048 (3GiB + 1MiB)  Kernel code start
     *  kernel_phys_end Kernel code end        -->  kernel_virt_end             Kernel code end
     *  0x??????        Initrd (modules) start -->  0x??????????                Initrd (modules) start
     *  initrd_end      Initrd (modules) end   -->  initrd_virt_end             Initrd (modules) end
     *  pile_phys_start Pile start             -->  pile_virt_start             Pile start
     *              KERNEL_PILE_SIZE                                KERNEL_PILE_SIZE
     *  pile_phys_end   Pile end               -->  pile_virt_end               Pile end
     *  heap_phys_start Heap start             -->  heap_virt_start             Heap start
     *              KERNEL_HEAP_SIZE                                KERNEL_HEAP_SIZE
     *  heap_phys_end   Heap end               -->  heap_virt_end               Heap end
     *
     *  We probably don't need to compute and store all of these values, but it helps with clarity (I think)
     */

    uint32_t code_phys_start = phys_start;
    uint32_t code_phys_end = phys_end;
    uint32_t code_virt_start = virtual_start;
    uint32_t code_virt_end = virtual_end;

    uint32_t initrd_phys_end = initrd_end;
    uint32_t initrd_phys_offset = initrd_phys_end - code_phys_end;

    uint32_t pile_phys_start = initrd_phys_end;
    uint32_t pile_phys_end = pile_phys_start + KERNEL_PILE_SIZE;
    uint32_t heap_phys_start = pile_phys_end;
    uint32_t heap_phys_end = heap_phys_start + KERNEL_HEAP_SIZE;
    uint32_t pile_virt_start = code_virt_end + initrd_phys_offset;
    uint32_t pile_virt_end = pile_virt_start + KERNEL_PILE_SIZE;
    uint32_t heap_virt_start = pile_virt_end;
    uint32_t heap_virt_end = heap_virt_start + KERNEL_HEAP_SIZE;

    uint32_t map_phys_start = ARCH_ALIGN_DOWN(code_phys_start, ARCH_PAGE_SIZE);
    uint32_t map_phys_end = ARCH_ALIGN_UP(heap_phys_end, ARCH_PAGE_SIZE);
    uint32_t map_virt_start = ARCH_ALIGN_DOWN(code_virt_start, ARCH_PAGE_SIZE);
    uint32_t map_virt_end = ARCH_ALIGN_UP(heap_virt_end, ARCH_PAGE_SIZE);

    pile_start = pile_virt_start;
    pile_ptr = pile_start;
    pile_end = pile_virt_end;

    // Create and map the directory
    kernel_directory = kmalloc_a(sizeof(page_directory_t));
    paging_map_4mb_dir(kernel_directory, map_phys_start, map_phys_end, map_virt_start, map_virt_end, KERNEL);

    arch_register_interrupt_handler(EXCEPTION_PAGE_FAULT, page_fault_handler);

    // Set directory
    uint32_t dir_physaddr = VIRTUAL_TO_PHYSICAL((uint32_t) kernel_directory);
    set_page_directory((page_directory_t *) dir_physaddr);

    /*
     * Allocate the frame bitmap before setting up the heap, since it doesn't take up much space
     * and it's best to keep as much of the heap free as possible. In addition it won't be freed
     * so keeping it in a non-freeable memory area, such as the pile, is fine.
     */

    // Use the aligned version as we don't want to discard any memory that falls between page boundaries
    num_frames = mem_aligned / PAGE_SIZE;
    // Align so as not to lose frames that fall between bitmap boundaries
    num_frames_aligned = num_frames % FRAMES_PER_BITMAP != 0 ? num_frames - (num_frames % FRAMES_PER_BITMAP) + FRAMES_PER_BITMAP : num_frames;
    size_t bitmap_size = (size_t) (num_frames_aligned / FRAMES_PER_BITMAP) * sizeof(FRAME_BITMAP_TYPE);
    frame_bitmaps = kmalloc(bitmap_size);
    if(frame_bitmaps) {
        memset(frame_bitmaps, 0, bitmap_size);
        paging_set_frames(map_phys_start, map_phys_end);
    }

    // Create the kernel heap
    kernel_heap = heap_create(heap_virt_start, heap_virt_end, true, true, HEAP_INDEX_SIZE);

    // If we couldn't allocate it in the pile, try to allocate it in the heap
    if(!frame_bitmaps) {
        frame_bitmaps = kmalloc(bitmap_size);
        if(frame_bitmaps) {
            memset(frame_bitmaps, 0, bitmap_size);
            paging_set_frames(map_phys_start, map_phys_end);
        } else PANIC("Couldn't allocate frame bitmap");
    }
}

page_directory_t* paging_create_page_dir(page_directory_t* base) {
    page_directory_t* dir = kmalloc_a(sizeof(page_directory_t));
    if(base) {
        // Copy all page directory entries from the base. Could probably be optimised to only copy
        // those that were actually populated
        for(uint32_t table_idx = 0; table_idx < PAGE_TABLE_ENTRIES_PER_DIRECTORY; table_idx++)
            memcpy(&dir->entries[table_idx], &base->entries[table_idx], sizeof(page_dir_entry_t));
    }
    return dir;
}

void paging_map_4mb_page(page_directory_t* dir, uint32_t page, uint32_t phys_addr, process_level_t level) {
    page_dir_entry_t* dir_entry = &dir->entries[page];
    dir_entry->accessed = 0;
    dir_entry->caching_disabled = 0;
    dir_entry->four_megabyte_pages = 1;
    dir_entry->available = 0;
    dir_entry->present = 1;
    dir_entry->user_level = level == USER ? 1 : 0;
    dir_entry->writable = 1;
    dir_entry->write_through = 1;
    ASSERT_PAGE_ALIGNED("table physical alignment", phys_addr);
    // Shift to isolate the top 10 bits (the 4MiB aligned physical address)
    dir_entry->page_physical_addr = phys_addr >> 22;
}

void paging_map_4mb_dir(page_directory_t* dir, uint32_t phys_start, uint32_t phys_end, uint32_t virtual_start, uint32_t virtual_end, process_level_t level) {
    if (phys_start > phys_end) PANIC("Improper start and end physical addresses");
    if(!ARCH_IS_PAGE_ALIGNED(phys_start) || !ARCH_IS_PAGE_ALIGNED(phys_end)) PANIC("Physical addresses are not page aligned");
    if (virtual_start > virtual_end) PANIC("Improper start and end virtual addresses");
    if(!ARCH_IS_PAGE_ALIGNED(virtual_start) || !ARCH_IS_PAGE_ALIGNED(virtual_end)) PANIC("Virtual addresses are not page aligned");
    if(virtual_end - virtual_start != phys_end - phys_start) PANIC("Virtual space is not equal to physical space");

    if (dir) {
        memset(dir, 0, sizeof(page_directory_t));
        uint32_t virtual_addr = virtual_start, phys_addr = phys_start;
        uint32_t page = virtual_addr / PAGE_SIZE;

        while (page < PAGE_TABLE_ENTRIES_PER_DIRECTORY && virtual_addr <= virtual_end) {
            paging_map_4mb_page(dir, page, phys_addr, level);
            phys_addr += PAGE_SIZE;
            virtual_addr += PAGE_SIZE;
            page++;
        }
    }
}
