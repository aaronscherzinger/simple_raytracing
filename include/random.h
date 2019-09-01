#pragma once

#include <stdlib.h>

// #TODO: replace by a thread-safe and fast random number generation
/// Returns a pseudo-random float in the range [0, 1)
inline float GetNextRandom()
{
#if __linux
    return drand48();
#elif WIN32
    static constexpr float max_rcp = 1.f / (static_cast<float>(RAND_MAX) + 1.f);

    return static_cast<float>(rand()) * max_rcp;
#else
    static_assert(false, "Unsupported platform");
#endif
}
