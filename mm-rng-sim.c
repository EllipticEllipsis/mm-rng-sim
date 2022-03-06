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
        printf("USAGE: %s SEED\n", argv[0]);
        printf("MM RNG Save Codes Simulator\n");
        printf("\n");
        printf("Arguments:\n");
        printf("  seed     a starting RNG value (hex, 8 digits)");
    }
    if (sscanf(argv[1], "%8X", &sRandInt) == 0) {
        fprintf(stderr, "error: entered characters are not a hex number\n");
        return 1;
    }
    printf("Starting RNG: 0x%08X\n", sRandInt);
    Sram_GenerateRandomSaveFields();
    printf("Bombers' code: %d%d%d%d%d\n", bomberCode[0], bomberCode[1], bomberCode[2], bomberCode[3], bomberCode[4]);
    
    convert_spider_code(spiderHouseMaskOrder);
    printf("Spider house order: %s\n", spiderHouseMaskString);

    printf("lottery numbers: %d%d%d %d%d%d %d%d%d\n", 
                lotteryCodes[0][0], lotteryCodes[0][1], lotteryCodes[0][2],
                lotteryCodes[1][0], lotteryCodes[1][1], lotteryCodes[1][2],
                lotteryCodes[2][0], lotteryCodes[2][1], lotteryCodes[2][2]);

    printf("   RNG value after generating this file: %8X\n", sRandInt);
    printf("\n");

    return 0;
}
// int main() {
//     // test();

//     // return 0;
//     char quit;
//     printf("MM RNG Save Codes Simulator\n");
//     printf("\n");
//     printf("Enter a starting RNG value (hex, 8 digits), or 'q' to quit: ");

//     while (1) {
//         if (scanf("%8X", &sRandInt) == 0) {
//             if (scanf("%c", &quit) != 0) {
//                 if (quit == 'q')
//                 return 0;
//             }
//             fprintf(stderr, "error: entered characters are not a hex number or 'q'\n");
//             return 1;
//         }
//         printf("Starting RNG: 0x%08X\n", sRandInt);
//         Sram_GenerateRandomSaveFields();
//         printf("Bombers' code: %d%d%d%d%d\n", bomberCode[0], bomberCode[1], bomberCode[2], bomberCode[3], bomberCode[4]);
        
//         convert_spider_code(spiderHouseMaskOrder);
//         printf("Spider house order: %s\n", spiderHouseMaskString);

//         printf("lottery numbers: %d%d%d %d%d%d %d%d%d\n", 
//                     lotteryCodes[0][0], lotteryCodes[0][1], lotteryCodes[0][2],
//                     lotteryCodes[1][0], lotteryCodes[1][1], lotteryCodes[1][2],
//                     lotteryCodes[2][0], lotteryCodes[2][1], lotteryCodes[2][2]);

//         printf("   RNG value after generating this file: %8X\n", sRandInt);
//         printf("\n");
//         printf("Enter another starting seed or quit with 'q': ");
//     }

//     return 0;
// }
