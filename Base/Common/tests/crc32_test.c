#include <stdio.h>
#include <string.h>

/* API from the archived Common CRC32.C implementation. */
extern unsigned CRC32_C(unsigned crc, const void *buffer, int length);

int main(void)
{
    const char *message = "123456789";
    unsigned crc = CRC32_C(0, message, (int)strlen(message));
    if (crc != 0xCBF43926U) {
        fprintf(stderr, "unexpected CRC32: %08x\n", crc);
        return 1;
    }
    if (CRC32_C(0, NULL, 0) != 0U)
        return 2;
    puts("CRC32 known vector: PASS");
    return 0;
}
