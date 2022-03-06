#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;

s8 lotteryCodes[3][3];
s8 spiderHouseMaskOrder[6];
s8 bomberCode[5];

u32 sRandInt;
u32 sRandFloat;

float hex_to_float(uint32_t hex) {
    int sign            = (hex & 0x80000000) ? -1 : 1;
    int biased_exponent = (hex & 0x7F800000) >> 23;
    uint32_t mantissa   =  hex & 0x007FFFFF;

    if (biased_exponent == 0xFF) {
        char str[10] = { 0 };

        sprintf(str, "0x%X", mantissa);

        if (mantissa == 0) {
            return INFINITY;
        } else if (mantissa & 0x400000) {
            return sign * nanf(str);
        } else {
            fprintf(stderr, "Calculation returned signalling NAN 0x%08X\n", hex);
            return sign * nanf(str);
        }
    } else {
        float fraction = mantissa / 0x1.0p23f;

        if (biased_exponent == 0) {
            return sign * powf(2.0f, -126) * fraction;
        } else {
            int exponent = biased_exponent - 127;

            return sign * powf(2.0f, exponent) * (1.0f + fraction);
        }
    }
}

// this function is lifted almost verbatim from mm decomp 
f32 Rand_ZeroOne(void) {
    sRandInt = (sRandInt * 1664525) + 1013904223;
    // printf("%08X\n", sRandInt);
    sRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&sRandFloat) - 1.0f;
}

// this function is lifted almost verbatim from mm decomp 
s16 Rand_S16Offset(s16 base, s16 range) {
    return (s16)(Rand_ZeroOne() * range) + base;
}

// this function is lifted almost verbatim from mm decomp 
void Sram_GenerateRandomSaveFields() {
    lotteryCodes[0][0] = Rand_S16Offset(0, 10);
    lotteryCodes[0][1] = Rand_S16Offset(0, 10);
    lotteryCodes[0][2] = Rand_S16Offset(0, 10);
    lotteryCodes[1][0] = Rand_S16Offset(0, 10);
    lotteryCodes[1][1] = Rand_S16Offset(0, 10);
    lotteryCodes[1][2] = Rand_S16Offset(0, 10);
    lotteryCodes[2][0] = Rand_S16Offset(0, 10);
    lotteryCodes[2][1] = Rand_S16Offset(0, 10);
    lotteryCodes[2][2] = Rand_S16Offset(0, 10);

    s16 i = 0;
    s16 prevSpiderDigit = Rand_S16Offset(0, 16) & 3;
    s16 max = 6;

    while (i != max) {
        s16 randSpiderDigit = Rand_S16Offset(0, 16) & 3;
        if (prevSpiderDigit != randSpiderDigit) {
            spiderHouseMaskOrder[i] = randSpiderDigit;
            i++;
            prevSpiderDigit = randSpiderDigit;
        }
    }

    s32 randBombers;

    do {
        randBombers = Rand_S16Offset(0, 6);
    } while (randBombers <= 0 || randBombers >= 6);

    bomberCode[0] = randBombers;

    i = 1;
    while (i != 5) {
        s32 k = 0;

        do {
            randBombers = Rand_S16Offset(0, 6);
        } while (randBombers <= 0 || randBombers >= 6);

        s16 index = 0;

        do {
            if (randBombers == bomberCode[index]) {
                k = 1;
            }
            index++;
        } while (index < i);

        if (k == 0) {
            bomberCode[i] = randBombers;
            i++;
        }
    }
}

char spiderHouseColors[] = { 'R', 'B', 'Y', 'G' };
char spiderHouseMaskString[7] = { 0 };

void convert_spider_code(s8* digits) {
    for (int i = 0; i < 6; i++) {
        spiderHouseMaskString[i] = spiderHouseColors[digits[i]];
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: %s LOTTERY_NUMBERS\n", argv[0]);
        printf("Brute-force searches RNG seed from lottery numbers.\n");
        printf("\n");
        printf("Arguments:\n");
        printf("  LOTTERY_NUMBERS      enter with no spaces, e.g. 123456789\n");
        printf("\n");
        return 1;
    }

    char numbers[10];

    if ((argc > 2) || (sscanf(argv[1], "%s", numbers) == 0)) {
        fprintf(stderr, "error: entered characters are not a single number\n");
        return 1;
    }

    printf("Input: %s\n", numbers);

    s32 digits[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    
    for (int i = 0; i < 9; i++) {
        digits[i] = numbers[i] - '0'; // Assumes ASCII input
        if (digits[i] < 0 || digits[i] > 9) {
            fprintf(stderr, "error: entered characters are not 9 decimal numbers\n");
            return 1;
        }
    }
    for (int i = 0; i < 9; i++) {
        printf("%d ", digits[i]);
    }
    printf("\n");

    uint64_t i = 0;
    for (i = 0; i <= UINT32_MAX; i++) {
        if ((i % (UINT32_MAX / 0x10 + 1)) == 0) {
            fprintf(stderr, "%08lX\n", i);
        }

        sRandInt = i;

        int j;
        for (j = 0; j < 9; j++) {
            if (digits[j] != Rand_S16Offset(0, 10)) {
                goto next; // Cheaper than using break and checking j, and continue's no good in a nested loop
            }
        }

        printf("matching seed: %08lX\n", i);
        next:;
    }

    return 0;
}
