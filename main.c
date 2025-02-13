#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "inmanip.h"

int
main(void)
{
    char buf[80] = { 0 };
    int32_t buflen = 0, count = 0;
    char c;
    while (true) {
        printf("[%d] $ %.*s\n", count, buflen, buf);
        count += 1;
        if (count > 10) { count = 0; }

        if (kbhit()) {
            do {
                c = getch();
                if (c == '\n') {
                    buflen = 0;
                } 
                else if (c == 127) {
                    if (buflen > 0) { buflen -= 1; }
                }
                else {
                    buf[buflen] = c;
                    buflen += 1;
                }
            } while(kbhit());
        }
    }
    return 0;
}
