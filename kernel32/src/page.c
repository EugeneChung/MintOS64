/**
 *  file    page.c
 *  date    2008/12/28
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   페이징에 관련된 각종 정보를 정의한 파일
 */

#include "page.h"

void kinit_page_tables() {
    kpml4_tentry_t *pml4_tentry;
    kpage_dirptr_tentry_t *page_dirptr_tentry;
    kpage_dir_entry_t *page_dir_entry;
    DWORD mapping_addr;
    DWORD page_entry_flags;
    int i;

    // PML4 테이블 생성
    // 첫 번째 엔트리 외에 나머지는 모두 0으로 초기화
    pml4_tentry = (kpml4_tentry_t *) 0x100000;
    // 페이지 엔트리를 유저 레벨로 설정하여 유저 레벨에서 접근 가능하도록 설정
    kfill_page_entry(&(pml4_tentry[0]), 0x101000, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US,
                     0, 0);
    for (i = 1; i < PAGE_MAX_ENTRY_COUNT; i++) {
        kfill_page_entry(&(pml4_tentry[i]), 0, 0, 0, 0);
    }

    // 페이지 디렉터리 포인터 테이블 생성
    // 하나의 PDPT로 512GByte까지 매핑 가능하므로 하나로 충분함
    // 64개의 엔트리를 설정하여 64GByte까지 매핑함
    page_dirptr_tentry = (kpage_dirptr_tentry_t *) 0x101000;
    for (i = 0; i < 64; i++) {
        // 페이지 엔트리를 유저 레벨로 설정하여 유저 레벨에서 접근 가능하도록 설정
        kfill_page_entry(&(page_dirptr_tentry[i]), 0x102000 + (i * PAGE_TABLE_SIZE),
                         PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US, 0, 0);
    }
    for (i = 64; i < PAGE_MAX_ENTRY_COUNT; i++) {
        kfill_page_entry(&(page_dirptr_tentry[i]), 0, 0, 0, 0);
    }

    // 페이지 디렉터리 테이블 생성
    // 하나의 페이지 디렉터리가 1GByte까지 매핑 가능
    // 여유있게 64개의 페이지 디렉터리를 생성하여 총 64GB까지 지원
    page_dir_entry = (kpage_dir_entry_t *) 0x102000;
    mapping_addr = 0;
    for (i = 0; i < PAGE_MAX_ENTRY_COUNT * 64; i++) {
        // 32비트로는 상위 어드레스를 표현할 수 없으므로, Mbyte 단위로 계산한 다음
        // 최종 결과를 다시 4Kbyte로 나누어 32비트 이상의 어드레스를 계산함
        kfill_page_entry(&(page_dir_entry[i]),
                         mapping_addr, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS,
                         (i * (PAGE_DEFAULT_SIZE >> 20)) >> 12, 0);

        mapping_addr += PAGE_DEFAULT_SIZE;
    }
}

void kfill_page_entry(kpage_tentry_t *entry, DWORD lower_base_addr, DWORD lower_flags, DWORD upper_base_addr,
                      DWORD upper_flags) {
    entry->lower = lower_base_addr | lower_flags;
    entry->upper = (upper_base_addr & 0xff) | upper_flags;
}