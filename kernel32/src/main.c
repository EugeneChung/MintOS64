#include "page.h"
#include "mode_switch.h"

void kprint_string(int x, int y, const char *string);
kbool_t kis_memory_enough();
kbool_t kinit_kernel64_area();
void kprepare_kernel64_image();

int main() {
    int line = 3;
    kdword_t eax, ebx, ecx, edx;
    char cpu_vendor[13] = {0, };

    kprint_string(0, line++, "Protected Mode C Language Kernel Start......[Pass]");

    kprint_string(0, line, "Minimum Memory Size Check...................[    ]");
    if (kis_memory_enough()) {
        kprint_string(45, line++, "Pass");
    } else {
        kprint_string(45, line++, "Fail");
        while (1);
    }

    kprint_string(0, line, "IA-32e Kernel Area Initialize...............[    ]");
    if (kinit_kernel64_area()) {
        kprint_string(45, line++, "Pass");
    } else {
        kprint_string(45, line++, "Fail");
        while (1);
    }

    kprint_string(0, line, "IA-32e Page Table Initialize................[    ]");
    kinit_page_tables();
    kprint_string(45, line++, "Pass");

    kread_cpuid(0, &eax, &ebx, &ecx, &edx);
    *((kdword_t *)cpu_vendor) = ebx;
    *((kdword_t *)cpu_vendor + 1) = edx;
    *((kdword_t *)cpu_vendor + 2) = ecx;
    cpu_vendor[12] = '\0';
    kprint_string(0, line, "CPU Vendor.......................[               ]");
    kprint_string(34, line++, cpu_vendor);

    kprint_string(0, line, "Prepare IA-32e Kernel Image.................[    ]");
    kprepare_kernel64_image();
    kprint_string(45, line++, "Pass");

    kprint_string(0, line, "Switching to IA-32e Mode..........................");
    kswitch_to_ia32e();

    while (1);
    return 0;
}

void kprint_string(int x, int y, const char *string) {
    kvideo_char_t *screen = (kvideo_char_t *) 0xb8000;
    int i;

    screen += 80 * y + x;
    for (i = 0; string[i] != '\0'; i++) {
        screen[i].value = string[i];
    }
}

kbool_t kis_memory_enough() {
    kdword_t *curr_address = (kdword_t *) 0x100000;

    // 0x4000000 = 64MB
    while ((kdword_t)curr_address < 0x4000000) {
        *curr_address = 0x12345678;

        // 0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
        // 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
        if (*curr_address != 0x12345678) {
            return FALSE;
        }

        curr_address += 0x1000; // Move 1MB
    }

    return TRUE;
}

kbool_t kinit_kernel64_area() {
    kdword_t *curr_address = (kdword_t *) 0x100000;

    while ((kdword_t)curr_address < 0x600000) {
        *curr_address = 0x00;

        // 0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
        // 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
        if (*curr_address != 0) {
            return FALSE;
        }

        curr_address++;
    }

    return TRUE;
}

void kprepare_kernel64_image() {
    kword_t total_sector_count, kernel32_sector_count, total_word_count;
    kdword_t *src_addr, *dest_addr;
    int i;

    // get the number of sectors from defined memory address set by bootloader
    total_sector_count = *((kword_t *) 0x7C05);
    kernel32_sector_count = *((kword_t *) 0x7C07);

    // copy kernel64 image from bootloader location to kernel64 location at the address 0x200000
    src_addr = (kdword_t *)(0x10000 + (kernel32_sector_count * 512));
    dest_addr = (kdword_t *) 0x200000;
    total_word_count = 512 * (total_sector_count - kernel32_sector_count) / 4;
    for (i = 0; i < total_word_count; i++) {
        *dest_addr = *src_addr;
        dest_addr++;
        src_addr++;
    }
}