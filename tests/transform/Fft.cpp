#include "aquila/global.h"
#include "aquila/source/ArrayData.h"
#include "aquila/source/generator/SineGenerator.h"
#include "aquila/transform/OouraFft.h"
#include <unittestpp.h>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

unsigned int findPeak(std::size_t arraySize,
                      Aquila::FrequencyType sampleFrequency,
                      Aquila::FrequencyType signalFrequency)
{
    Aquila::SineGenerator generator(sampleFrequency);
    generator.setAmplitude(64).setFrequency(signalFrequency).generate(arraySize);

    Aquila::ComplexType* spectrum = new Aquila::ComplexType[arraySize];
    Aquila::OouraFft fft(arraySize);
    fft.fft(generator.toArray(), spectrum);

    // "iterator" pointing to highest spectrum peak (comparing by magnitude)
    // the pointer difference is the number of spectral peak
    auto peakPos = std::max_element(
        spectrum,
        spectrum + arraySize / 2,
        [] (Aquila::ComplexType x, Aquila::ComplexType y) -> bool {
            return std::abs(x) < std::abs(y);
        }
    );

    unsigned int d = std::distance(spectrum, peakPos);
    delete [] spectrum;

    return d;
}

SUITE(Fft)
{
    TEST(Delta)
    {
        const std::size_t SIZE = 8;
        Aquila::SampleType testArray[SIZE] = {1, 0, 0, 0, 0, 0, 0, 0};
        Aquila::ArrayData<> data(testArray, SIZE, 22050);
        Aquila::ComplexType* spectrum = new Aquila::ComplexType[SIZE];
        Aquila::OouraFft fft(SIZE);
        fft.fft(data.toArray(), spectrum);

        std::for_each(spectrum, spectrum + SIZE, [](Aquila::ComplexType x) {
            CHECK_CLOSE(1.0, std::abs(x), 0.000001);
        });

        delete [] spectrum;
    }

    TEST(ConstSignal)
    {
        const std::size_t SIZE = 8;
        Aquila::SampleType testArray[SIZE] = {1, 1, 1, 1, 1, 1, 1, 1};
        Aquila::ArrayData<> data(testArray, SIZE, 22050);
        Aquila::ComplexType* spectrum = new Aquila::ComplexType[SIZE];
        Aquila::OouraFft fft(SIZE);
        fft.fft(data.toArray(), spectrum);

        CHECK_CLOSE(1.0 * SIZE, std::abs(spectrum[0]), 0.000001);
        std::for_each(spectrum + 1, spectrum + SIZE, [](Aquila::ComplexType x) {
            CHECK_CLOSE(0.0, std::abs(x), 0.000001);
        });

        delete [] spectrum;
    }

    TEST(SinePeakDetect1)
    {
        unsigned int peakPosition = findPeak(64, 8000, 1000);
        CHECK_EQUAL(8u, peakPosition);
    }

    TEST(SinePeakDetect2)
    {
        unsigned int peakPosition = findPeak(64, 8000, 2000);
        CHECK_EQUAL(16u, peakPosition);
    }

    TEST(SinePeakDetect3)
    {
        unsigned int peakPosition = findPeak(64, 8000, 3999);
        CHECK_EQUAL(31u, peakPosition);
    }

    TEST(SinePeakDetect4)
    {
        unsigned int peakPosition = findPeak(64, 8000, 1);
        CHECK_EQUAL(0u, peakPosition);
    }

    TEST(SinePeakDetect5)
    {
        unsigned int peakPosition = findPeak(1024, 8000, 1000);
        CHECK_EQUAL(128u, peakPosition);
    }

    TEST(SinePeakDetect6)
    {
        unsigned int peakPosition = findPeak(1024, 8000, 2000);
        CHECK_EQUAL(256u, peakPosition);
    }

    TEST(SinePeakDetect7)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 1000);
        CHECK_EQUAL(23u, peakPosition);
    }

    TEST(SinePeakDetect8)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 2000);
        CHECK_EQUAL(46u, peakPosition);
    }

    TEST(SinePeakDetect9)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 4567);
        CHECK_EQUAL(106u, peakPosition);
    }

    TEST(SinePeakDetect10)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 10000);
        CHECK_EQUAL(232u, peakPosition);
    }

    TEST(SinePeakDetect11)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 15000);
        CHECK_EQUAL(348u, peakPosition);
    }

    TEST(SinePeakDetect12)
    {
        unsigned int peakPosition = findPeak(1024, 44100, 21204);
        CHECK_EQUAL(492u, peakPosition);
    }
}
