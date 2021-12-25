#include "types.h"

void kprint_string(int x, int y, const char *string);

int main() {
    int line = 10;

    kprint_string(0, line++, "IA-32e Mode C Language Kernel Start.........[Pass]");

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
