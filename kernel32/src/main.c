#include "types.h"

void kprint_string(int x, int y, const char *string);

int main(void) {
    kprint_string(0, 3, "C Language Kernel Started");
    while (1);
    return 0;
}

void kprint_string(int x, int y, const char *string) {
    kvideo_char_t *screen = (kvideo_char_t *) 0xb8000;
    int i;

    screen += 80 * y + x;
    for (i = 0; *string != '\0'; i++) {
        screen[i].value = string[i];
    }
}