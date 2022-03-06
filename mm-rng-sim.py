#! usr/env/bin python3
import argparse
import sys

def hex_to_float(hex: int) -> float:
    sign: int            = -1 if (hex & 0x80000000) else 1
    biased_exponent: int = (hex & 0x7F800000) >> 23
    mantissa: int        =  hex & 0x007FFFFF

    if biased_exponent == 0xFF:
        if mantissa == 0:
            return float('inf')
        elif mantissa & 0x400000:
            print(f"Calculation returned quiet NAN 0x{hex:08X}", file=sys.stderr)
        else:
            print(f"Calculation returned signalling NAN 0x{hex:08X}", file=sys.stderr)
        return float('nan')

    else:
        fraction: float = mantissa / (1 << 23)

        if biased_exponent == 0:
            return sign * pow(2.0, -126) * fraction
        else:
            exponent: int = biased_exponent - 127

            return sign * pow(2.0, exponent) * (1.0 + fraction)


lotteryCodes = [[0,0,0],[0,0,0],[0,0,0]]
spiderHouseMaskOrder = [0,0,0,0,0,0]
bomberCode = [0,0,0,0,0]

sRandInt: int = 1
sRandFloat: int = 1

# this function is a reimplementation of the one from MM decomp
def Rand_ZeroOne() -> float:
    global sRandInt

    sRandInt = ((sRandInt * 1664525) + 1013904223) & 0xFFFFFFFF
    # print(f"{sRandInt:08X}")
    sRandFloat = ((sRandInt >> 9) | 0x3F800000)
    return hex_to_float(sRandFloat) - 1.0


# this function is a reimplementation of the one from MM decomp
def Rand_S16Offset(base: int, range: int) -> int:
    return int(Rand_ZeroOne() * range) + base


# this function is a reimplementation of the one from MM decomp: it is deliberatly un-Pythonic to
# make comparison with the C easier
def Sram_GenerateRandomSaveFields():
    global lotteryCodes
    global bomberCode
    global spiderHouseMaskOrder

    lotteryCodes[0][0] = Rand_S16Offset(0, 10)
    lotteryCodes[0][1] = Rand_S16Offset(0, 10)
    lotteryCodes[0][2] = Rand_S16Offset(0, 10)
    lotteryCodes[1][0] = Rand_S16Offset(0, 10)
    lotteryCodes[1][1] = Rand_S16Offset(0, 10)
    lotteryCodes[1][2] = Rand_S16Offset(0, 10)
    lotteryCodes[2][0] = Rand_S16Offset(0, 10)
    lotteryCodes[2][1] = Rand_S16Offset(0, 10)
    lotteryCodes[2][2] = Rand_S16Offset(0, 10)

    i: int = 0
    prevSpiderDigit: int = Rand_S16Offset(0, 16) & 3
    max: int = 6

    while i != max:
        randSpiderDigit: int = Rand_S16Offset(0, 16) & 3
        if (prevSpiderDigit != randSpiderDigit):
            spiderHouseMaskOrder[i] = randSpiderDigit
            i += 1
            prevSpiderDigit = randSpiderDigit

    randBombers: int

    while True:
        randBombers = Rand_S16Offset(0, 6)
        if not (randBombers <= 0 or randBombers >= 6):
            break

    bomberCode[0] = randBombers

    i = 1
    while i != 5:
        k: int = 0

        while True:
            randBombers = Rand_S16Offset(0, 6)
            if not (randBombers <= 0 or randBombers >= 6):
                break

        index: int = 0

        while True:
            if randBombers == bomberCode[index]:
                k = 1
            
            index += 1
            if not (index < i):
                break

        if k == 0:
            bomberCode[i] = randBombers
            i += 1


spiderHouseColors = [ 'R', 'B', 'Y', 'G' ]

def convert_spider_code(digits):
    return [spiderHouseColors[digit] for digit in digits]


def main():
    description = "MM save file RNG calculator. Given a starting RNG value, calculate "
    epilog = ""

    parser = argparse.ArgumentParser(description=description, epilog=epilog, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("seed", help="Initial value.")
    
    args = parser.parse_args()

    global sRandInt
    sRandInt = int(args.seed, 16)

    print(f"Starting RNG: 0x{sRandInt:08X}")

    Sram_GenerateRandomSaveFields()

    print(f"Bombers' code: {''.join([str(a) for a in bomberCode])}")
    
    print(f"Spider house order: {''.join(convert_spider_code(spiderHouseMaskOrder))}")

    print("lottery numbers: ", end="")
    for day in lotteryCodes:
        print("".join([str(d) for d in day]), end=" ")
    print()

    print(f"    RNG value after generating this file: {sRandInt:08X}")
    print()

    

if __name__ == "__main__":
    main()