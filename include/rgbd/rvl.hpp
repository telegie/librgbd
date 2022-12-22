/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "constants.hpp"

// This algorithm is from
// Wilson, A. D. (2017, October). Fast lossless depth image compression.
// In Proceedings of the 2017 ACM International Conference on Interactive
// Surfaces and Spaces (pp. 100-105). ACM.
namespace rgbd
{
// Code inside this namespace is from the RVL paper (Wilson, 2017).
// The code has been modified to be thread-safe (i.e. removed global variables).
// Also, using int64_t instead of the original ints to prevent overflow when using with
// int32_t inputs.
namespace wilson
{
void EncodeVLE(int64_t value, int*& pBuffer, int& word, int& nibblesWritten);
int64_t DecodeVLE(int*& pBuffer, int& word, int& nibblesWritten);
template <class T>
size_t CompressRVL(T* input, char* output, int64_t numPixels)
{
    int* buffer{reinterpret_cast<int*>(output)};
    int* pBuffer{reinterpret_cast<int*>(output)};
    int word{0};
    int nibblesWritten{0};
    T* end{input + numPixels};
    T previous{0};
    while (input != end) {
        int64_t zeros{0};
        int64_t nonzeros{0};
        while((input != end) && !*input) {
            ++input;
            ++zeros;
        }
        EncodeVLE(zeros, pBuffer, word, nibblesWritten); // number of zeros
        T* p{input};
        while((p != end) && *p++) {
            ++nonzeros;
        }
        EncodeVLE(nonzeros, pBuffer, word, nibblesWritten); // number of nonzeros
        for (int i{0}; i < nonzeros; ++i) {
#pragma warning(push)
#pragma warning(disable : 28182)
            T current{*input++};
#pragma warning(pop)
            int64_t delta{current - previous};
            int64_t positive{(delta << 1) ^ (delta >> 63)};
            EncodeVLE(positive, pBuffer, word, nibblesWritten); // nonzero value
            previous = current;
        }
    }

    if (nibblesWritten) // last few values
        *pBuffer++ = word << 4 * (8 - nibblesWritten);

    return size_t((char*)pBuffer - (char*)buffer); // num bytes
}

template <class T>
void DecompressRVL(char* input, T* output, int64_t numPixels)
{
    int* pBuffer{reinterpret_cast<int*>(input)};
    int word{0};
    int nibblesWritten{0};
    T current;
    T previous{0};
    int64_t numPixelsToDecode{numPixels};
    while (numPixelsToDecode > 0) {
        int64_t zeros{DecodeVLE(pBuffer, word, nibblesWritten)}; // number of zeros
        numPixelsToDecode -= zeros;
        while (zeros) {
            *output++ = 0;
            --zeros;
        }
        int64_t nonzeros{DecodeVLE(pBuffer, word, nibblesWritten)}; // number of nonzeros
        numPixelsToDecode -= nonzeros;
        while (nonzeros) {
            int64_t positive{DecodeVLE(pBuffer, word, nibblesWritten)}; // nonzero value
            int64_t delta{(positive >> 1) ^ -(positive & 1)};
#pragma warning(push)
#pragma warning(disable : 4244)
            current = previous + delta;
#pragma warning(pop)
            *output++ = current;
            previous = current;
            --nonzeros;
        }
    }
}
} // namespace wilson

namespace rvl
{
// Type T has to be signed, not unsigned, to work with TRVL.
template <class T>
Bytes compress(const gsl::span<const T> input) noexcept
{
    // Theoretically, if all input are non-zero and has a number that makes them
    // the longest in VLE encoding, it would be 24 bits for int16_t values
    // (since 16 bits can be turned into 6 3-bit chunks with a bit in front of each
    // chunk)
    // For int32_t, it would be 44 bits.
    // For int64_t, it would be 88 bits.
    // They all become less than 1.5 times longer than they originally were.
    // So multiplying 3 and dividing 2 below.
    Bytes output(gsl::narrow<size_t>(input.size() * 3 / 2 * sizeof(T)));
    size_t size{wilson::CompressRVL(const_cast<T*>(input.data()),
                                    reinterpret_cast<char*>(output.data()),
                                    gsl::narrow<int64_t>(input.size()))};
    output.resize(size);
    output.shrink_to_fit();
    return output;
}

template <class T>
vector<T> decompress(const gsl::span<const std::byte> input, const int64_t num_pixels) noexcept
{
    vector<T> output(num_pixels);
    wilson::DecompressRVL(
        reinterpret_cast<char*>(const_cast<std::byte*>(input.data())), output.data(), num_pixels);
    return output;
}
} // namespace rvl
} // namespace rgbd
