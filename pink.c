#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>

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

    FILE* randomFile = fopen("/dev/urandom", "rb");
    fread(
        randomBytes, sizeof(*randomBytes),
        sizeof(randomBytes)/sizeof(*randomBytes),
        randomFile
    );
    fclose(randomFile);
    printf("this may take some time…\n");
    for (uint32_t s = 0; s < numSeconds*sampleRate; s++) {
        float sum = 0;
        for (uint32_t f = 10; f < sizeof(randomBytes)/sizeof(*randomBytes); f++) {
            const float r = randomBytes[f];
            const uint8_t maxRandom = -1;
            static_assert(
                sizeof(maxRandom) == sizeof(*randomBytes),
                "maxRandom has the wrong type"
            );
            const float offset = r*2*M_PI/maxRandom;
            const float amp = 1./f;
            const float Hz = s*2*M_PI/sampleRate/numSeconds;
            sum += amp * sinf(f*Hz + offset);
        }
        static int progress = 0;
        if ((int)(10. * s / sampleRate / numSeconds) > progress) {
            progress = 10 * s / sampleRate / numSeconds;
            printf("%3d%% done\n", 10*progress);
        }
        for (int c = 0; c < numChannels; c++) {
            static_assert(
                sizeof(sample) == sizeof(int16_t),
                "the assumtion that sample is a 16 bit signed integer was wrong"
            );
            samples[s*numChannels + c] = INT16_MAX * sum;
        }
    }

    if (writeBuffer(samples, duration/sampleSize, sampleRate, "pink.wav")) {
        fprintf(stderr, "writeBuffer failed\n");
    }

    free(samples);
    return EXIT_SUCCESS;
}
