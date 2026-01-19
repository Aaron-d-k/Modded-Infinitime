#pragma once
#include <cinttypes>

struct fastRNG
{
    uint32_t x;

    fastRNG(uint32_t rng_init)
    {
        x=rng_init;
    }

    void next_state()
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
    }
    
    uint32_t operator()()
    {
        next_state();
        return x;
    }
};
