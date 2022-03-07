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
    sRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&sRandFloat) - 1.0f;
}

// Both operations involved in the new seed calculation are invertible modulo 1 << 32, this function is the inverse
void Rand_Reverse(void) {
    sRandInt = (sRandInt * 4276115653) + 634785765;
}

int main(int argc, char** argv) {
    int steps_back;

    if (argc < 2) {
        printf("USAGE: %s SEED [STEPS_BACK]\n", argv[0]);
        printf("Brute-force searches for a seed STEPS_BACK before the .\n");
        printf("\n");
        printf("Arguments:\n");
        printf("  SEED           seed from which to start seaching. In hex\n");
        printf("  STEPS_BACK     number to step backwards, in dec. Default: 1\n");
        printf("\n");
        return 1;
    }


    if (sscanf(argv[1], "%X", &sRandInt) == 0) {
        fprintf(stderr, "error: entered characters in first argument are not a hexadecimal number\n");
        return 1;
    }

    if (argc == 2) {
        steps_back = 1;
    } else if (sscanf(argv[2], "%d", &steps_back) == 0) {
        fprintf(stderr, "error: entered characters in second argument are not a decimal number\n");
        return 1;
    } else if (argc > 3) {
        fprintf(stderr, "error: too many arguments passed\n");
        return 1;
    }
    
    printf("Starting RNG: %08X\n", sRandInt);
    printf("Reversing RNG by: %d\n", steps_back);

    for (int i = 0; i < steps_back; i++) {
        Rand_Reverse();

        printf("%2d steps before: %08X\n", i + 1, sRandInt);
    }

    return 0;
}
