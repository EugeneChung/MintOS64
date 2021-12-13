#include "types.h"

void kprint_string(int x, int y, const char *string);
BOOL kis_memory_enough();
BOOL kinit_kernel64_area();

int main(void) {
    kprint_string(0, 3, "Protected Mode C Language Kernel Start......[Pass]");

    kprint_string(0, 4, "Minimum Memory Size Check...................[    ]");
    if (kis_memory_enough()) {
        kprint_string(45, 4, "Pass");
    } else {
        kprint_string(45, 4, "Fail");
        while (1);
    }

    kprint_string(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
    if (kinit_kernel64_area()) {
        kprint_string(45, 5, "Pass");
    } else {
        kprint_string(45, 5, "Fail");
        while (1);
    }

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

BOOL kis_memory_enough() {
    DWORD *curr_address = (DWORD *) 0x100000;

    // 0x4000000 = 64MB
    while ((DWORD)curr_address < 0x4000000) {
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

BOOL kinit_kernel64_area() {
    DWORD *curr_address = (DWORD *) 0x100000;

    while ((DWORD)curr_address < 0x600000) {
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