/**
 *  file    page.h
 *  date    2008/12/28
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   페이징에 관련된 각종 정보를 정의한 파일
 */

#ifndef MINTOS64_PAGE_H
#define MINTOS64_PAGE_H

#include "types.h"

#define PAGE_FLAGS_P         0x00000001  // Present
#define PAGE_FLAGS_RW        0x00000002  // Read/Write
#define PAGE_FLAGS_US        0x00000004  // User/Supervisor
#define PAGE_FLAGS_PWT       0x00000008  // Page Level Write-through
#define PAGE_FLAGS_PCD       0x00000010  // Page Level Cache Disable
#define PAGE_FLAGS_A         0x00000020  // Accessed
#define PAGE_FLAGS_D         0x00000040  // Dirty
#define PAGE_FLAGS_PS        0x00000080  // Page Size
#define PAGE_FLAGS_G         0x00000100  // Global
#define PAGE_FLAGS_PAT       0x00001000  // Page Attribute Table Index
#define PAGE_FLAGS_EXB       0x80000000  // Execute Disable Bit
#define PAGE_FLAGS_DEFAULT   (PAGE_FLAGS_P | PAGE_FLAGS_RW)

#define PAGE_TABLE_SIZE      0x1000   // 4KB
#define PAGE_MAX_ENTRY_COUNT 512
#define PAGE_DEFAULT_SIZE    0x200000 // 2MB

#pragma pack(push, 1)

/**
 * tentry = table entry
 */
typedef struct kpage_tentry {
    // PML4T와 PDPTE의 경우
    // 1 비트 P, RW, US, PWT, PCD, A, 3 비트 Reserved, 3 비트 Avail,
    // 20 비트 Base Address
    // PDE의 경우
    // 1 비트 P, RW, US, PWT, PCD, A, D, 1, G, 3 비트 Avail, 1 비트 PAT, 8 비트 Avail,
    // 11 비트 Base Address
    kdword_t lower;
    // 8 비트 Upper BaseAddress, 12 비트 Reserved, 11 비트 Avail, 1 비트 EXB
    kdword_t upper;
} kpml4_tentry_t, kpage_dirptr_tentry_t, kpage_dir_entry_t, kpage_tentry_t;

#pragma pack(pop)

void kinit_page_tables();
void kfill_page_entry(kpage_tentry_t *entry, kdword_t lower_base_addr, kdword_t lower_flags, kdword_t upper_base_addr,
                      kdword_t upper_flags);

#endif //MINTOS64_PAGE_H
