#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
// #include <math.h>

#include "wav.h"

static uint8_t randomBytes[20000];

int main() {
    srand(15);
    const uint32_t sampleRate = 44100;
    const size_t sampleSize = sizeof(sample);
    const size_t second = sampleSize*numChannels*sampleRate;
    const uint32_t numSeconds = 10;
    const uint32_t duration = numSeconds*second;

    sample* samples = malloc(duration);
    // memset(samples, 0, 1*seconds);

    assert(sizeof(*samples) == sampleSize);

    for (uint32_t s = 0; s < numSeconds*sampleRate; s++) {
        for (int c = 0; c < numChannels; c++) {
            static_assert(
                sizeof(sample) == sizeof(int16_t),
                "the assumtion that sample is a 16 bit signed integer was wrong"
            );
            samples[s*numChannels + c] = 0;
        }
    }

    if (writeBuffer(samples, duration/sampleSize, sampleRate, "zero.wav")) {
        fprintf(stderr, "writeBuffer failed\n");
    }

    free(samples);
    return EXIT_SUCCESS;
}
