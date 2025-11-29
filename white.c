#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
// #include <math.h>

enum MagicBytes : uint32_t {
    RIFF = 0x46464952,
    WAVE = 0x45564157,
    fmt = 0x20746D66,
    data = 0x61746164,
};

enum AudioFormat : uint16_t {
    AUDIO_FORMAT_PCM_integer = 1,
    AUDIO_FORMAT_IEEE_754_float = 3,
};

typedef int16_t sample;

static const uint16_t numChannels = 2;

// https://en.wikipedia.org/wiki/WAV#WAV_file_header
struct RIFF_WAVE_Header{
    // Master RIFF Chunk
    enum MagicBytes FileTypeBlocID;
    uint32_t FileSize;
    enum MagicBytes FileFormatID;
    // Chunk describing the data format
    enum MagicBytes FormatBlocID;
    uint32_t BlocSize;
    enum AudioFormat AudioFormat;
    uint16_t NbrChannels;
    uint32_t Frequency;
    uint32_t BytePerSec;
    uint16_t BytePerBloc;
    uint16_t BitsPerSample;
    // Chunk containing the sampled data
    enum MagicBytes DataBlocID;
    uint32_t DataSize;
    // void* SampledData;
    // uint8 SampledData[];
    // float SampledData[];
};

static void writeWAVHeader(FILE* file, uint32_t sampleRate, uint32_t numSamples) {
    const uint32_t sampleSize = sizeof(sample);
    const uint32_t dataSize = sampleSize*numSamples;
    struct RIFF_WAVE_Header header = {
        RIFF, dataSize+sizeof(struct RIFF_WAVE_Header)-8,
        WAVE, fmt, 16,
        AUDIO_FORMAT_PCM_integer, numChannels,
        sampleRate, sampleRate*sampleSize*numChannels,
        sampleSize*numChannels, sampleSize*8,
        data, dataSize
    };

    assert(sampleSize == (
        header.AudioFormat == AUDIO_FORMAT_PCM_integer ?
        sizeof(uint16_t) : sizeof(float)
    ));
    assert(fwrite(&header, sizeof(header), 1, file) == 1);

}

static int writeBuffer(sample* samples, uint32_t numSamples, uint32_t sampleRate, const char* filename) {
    int ret = EXIT_SUCCESS;
    FILE* file = fopen(filename, "w+");
    if (!file) {
        return EXIT_FAILURE;
    }
    
    writeWAVHeader(file, sampleRate, numSamples);

    size_t written = fwrite(samples, sizeof(*samples), numSamples, file);
    fprintf(stdout, "%zu\n", written);

    assert(written == numSamples);

// close_file:
    fclose(file);
    return ret;
}

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
        float sum = rand()/(float)RAND_MAX - 0.5f;
        for (int c = 0; c < numChannels; c++) {
            static_assert(
                sizeof(sample) == sizeof(int16_t),
                "the assumtion that sample is a 16 bit signed integer was wrong"
            );
            samples[s*numChannels + c] = INT16_MAX * sum;
        }
    }

    if (writeBuffer(samples, duration/sampleSize, sampleRate, "white.wav")) {
        fprintf(stderr, "writeBuffer failed\n");
    }

    free(samples);
    return EXIT_SUCCESS;
}
