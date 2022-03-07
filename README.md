# RNG-related tools for Majora's Mask

## mm-rng-sim

Given a seed (8-digit hex number), calculate the lottery numbers, Bombers' code, and Oceanside Spider House mask order.

Available in both C and Python, both should give identical outputs. If not, the C program uses almost exactly the code from actual MM decomp, so should be taken as accurate over the Python.

## find-rng-seed

Brute-forces possible RNG values from the lottery numbers, which are by far the most informative (there are 10^9 possibilities, whereas there are only 120 Bombers' codes and 5 * 4^5 = 1280 spider house orderings). Is slow due to single-threading, but should be efficient enough to not be *too* slow.

## step-back-rng

Print the previous n RNG values. This can be calculated using the inverse of the linear congruential generator.

## advance-rng

Print the next n RNG values.
