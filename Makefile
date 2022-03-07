CFLAGS := -Wall -Wextra -pedantic -std=c99 -Os -g
LIB := -lm
PROGRAMS := mm-rng-sim.elf find-rng-seed.elf advance-rng.elf step-back-rng.elf

ifeq ($(shell command -v clang >/dev/null 2>&1; echo $$?),0)
  CC := clang
else
  CC := gcc
endif

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS)

%.elf: %.c
	$(CC) $(CFLAGS) $(LIB) $< -o $@
