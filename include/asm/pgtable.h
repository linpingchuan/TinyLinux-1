/*
 * include/asm/pgtable.h
 * Copyright (C) <2014>  <@RKX1209>
 */

#ifndef _I386_PGTABLE_H
#define _I386_PGTABLE_H

#include <abyon/init_task.h>

#include <asm/fixmap.h>
#include <asm/page.h>
#include <asm/system.h>

extern pgd_t swapper_pg_dir[1024];

#define VMALLOC_START (896 * 1024 * 1024)
#define VMALLOC_END FIXADDR_START - 2 * PAGE_SIZE

#define PTRS_PER_PGD 1024 //entries per one pgd
#define PTRS_PER_PTE 1024 //entries per one pte

/* <Page directory table> */
#define PGDIR_SHIFT 22
#define PGDIR_SIZE  (1UL << PGDIR_SHIFT)
#define PGDIR_MASK  (~(PGDIR_SIZE - 1))

/* Get pgd index from virtual address */
#define pgd_index(addr) (((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

/* Get pgd */
#define pgd_offset(mm,addr) ((mm)->pgd + pgd_index(addr))
#define pgd_offset_k(addr) pgd_offset(&init_mm,addr)

/* Page table */
#define PTE_SHIFT
#define pte_index(addr) (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

/* Page directory entry attribute */
#define _PAGE_PRESENT	0x001
#define _PAGE_RW	0x002
#define _PAGE_USER	0x004
#define _PAGE_PWT	0x008
#define _PAGE_PCD	0x010
#define _PAGE_ACCESSED	0x020
#define _PAGE_DIRTY	0x040
#define _PAGE_PSE	0x080
#define _PAGE_GLOBAL	0x100
#define _PAGE_UNUSED1	0x200
#define _PAGE_UNUSED2	0x400
#define _PAGE_UNUSED3	0x800

#define _PAGE_FILE	0x040	
#define _PAGE_PROTNONE	0x080	
#define _PAGE_NX	0

#define _PAGE_TABLE	(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED | _PAGE_DIRTY)

#define _PAGE_SHARED \
	(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)

#define _PAGE_SHARED_EXEC \
	(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)

#define _PAGE_KERNEL \
  (_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_NX)
#define _PAGE_KERNEL_EXEC \
  (_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED)

#define PAGE_SHARED (pgprot_t){_PAGE_SHARED}
#define PAGE_SHARED_EXEC (pgprot_t){_PAGE_SHARED_EXEC}
#define PAGE_KERNEL (pgprot_t){_PAGE_KERNEL}
#define PAGE_KERNEL_EXEC (pgprot_t){_PAGE_KERNEL_EXEC}
  
/* Generate PDE or PTE from pfn(page frame number) */
#define pfn_pte(pfn,prot) (pte_t){((pfn) << PAGE_SHIFT) | prot.pgprot}
#define pte_pfn(x) ((unsigned long)(((x).pte_low >> PAGE_SHIFT)))

#define pgd_none(x) ((x).pgd == 0)

#define pte_virtual_addr(pgdt,addr) \
  ((pte_t *) pa_to_va((pgdt)->pgd & PAGE_MASK) + pte_index(addr))

/* #define pgd_pa_addr(pgdat,addr)						\ */
/*   ((pte_t *) (((pgdat).pgd ^ ((pgdat).pgd & PAGE_MASK)) + pgd_index(addr))) */

/* #define pte_pa_addr(ptedat,addr) \ */
/*   ((unsigned long) (((ptedat).pte_low ^ ((ptedat).pte_low & PAGE_MASK)) + pte_index(addr))) */

#define pgd_pa_addr(pgdat,addr)						\
  ((pte_t *) (((pgdat).pgd & 0xfffff000) + pgd_index(addr)))

#define pte_pa_addr(ptedat,addr) \
  ((unsigned long) (((ptedat).pte_low & 0xfffff000) + pte_index(addr)))

/* Change physical address to virtual address (Straight map area) */
#define pa_to_va(addr) (void *)((unsigned long)(addr))


#define PFN_UP(x) (((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x) ((x) >> PAGE_SHIFT)

/* set_pte: Gurantee the order of memory access
 * not to have late access to PTE.
 * Use memory barrier.
 */
static inline void set_pte(pte_t *ptep,pte_t pte){
  ptep->pte_low = pte.pte_low;
  //smp_wmb(); //Write memory barrier
}

#endif
