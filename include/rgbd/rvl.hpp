/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "constants.hpp"

// This algorithm is from
// Wilson, A. D. (2017, October). Fast lossless depth image compression.
// In Proceedings of the 2017 ACM International Conference on Interactive
// Surfaces and Spaces (pp. 100-105). ACM.
namespace tg
{
// Code inside this namespace is from the RVL paper (Wilson, 2017).
// The code has been modified to be thread-safe (i.e. removed global variables).
namespace wilson
{
void EncodeVLE(int value, int*& pBuffer, int& word, int& nibblesWritten);
int DecodeVLE(int*& pBuffer, int& word, int& nibblesWritten);
template <class T>
int CompressRVL(T* input, char* output, int numPixels)
{
    int* buffer = (int*)output;
    int* pBuffer = (int*)output;
    int word = 0;
    int nibblesWritten = 0;
    T* end = input + numPixels;
    T previous = 0;
    while (input != end) {
        int zeros = 0, nonzeros = 0;
        for (; (input != end) && !*input; input++, zeros++)
            ;
        EncodeVLE(zeros, pBuffer, word, nibblesWritten); // number of zeros
        for (T* p = input; (p != end) && *p++; nonzeros++)
            ;
        EncodeVLE(nonzeros, pBuffer, word,
                  nibblesWritten); // number of nonzeros
        for (int i = 0; i < nonzeros; i++) {
#pragma warning(push)
#pragma warning(disable : 28182)
            T current = *input++;
#pragma warning(pop)
            int delta = current - previous;
            int positive = (delta << 1) ^ (delta >> 31);
            EncodeVLE(positive, pBuffer, word, nibblesWritten); // nonzero value
            previous = current;
        }
    }

    if (nibblesWritten) // last few values
        *pBuffer++ = word << 4 * (8 - nibblesWritten);

    return int((char*)pBuffer - (char*)buffer); // num bytes
}

template <class T>
void DecompressRVL(char* input, T* output, int numPixels)
{
    // int* buffer = (int*)input;
    int* pBuffer = (int*)input;
    int word = 0;
    int nibblesWritten = 0;
    T current, previous = 0;
    int numPixelsToDecode = numPixels;
    while (numPixelsToDecode) {
        int zeros = DecodeVLE(pBuffer, word, nibblesWritten); // number of zeros
        numPixelsToDecode -= zeros;
        for (; zeros; zeros--)
            *output++ = 0;
        int nonzeros = DecodeVLE(pBuffer, word, nibblesWritten); // number of nonzeros
        numPixelsToDecode -= nonzeros;
        for (; nonzeros; nonzeros--) {
            int positive = DecodeVLE(pBuffer, word, nibblesWritten); // nonzero value
            int delta = (positive >> 1) ^ -(positive & 1);
#pragma warning(push)
#pragma warning(disable : 4244)
            current = previous + delta;
#pragma warning(pop)
            *output++ = current;
            previous = current;
        }
    }
}
} // namespace wilson

namespace rvl
{
// It has to be int16_t not uint16_t to work with TRVL.
template <class T>
Bytes compress(const gsl::span<const T> input) noexcept
{
    // Theoretically, if all input are non-zero and has a number that makes them
    // the longest in VLE encoding, which would be 24 bits for int16_t values
    // (since 16 bits can be turned into 6 3-bit chunks with a bit in front of each
    // chunk), in worst case, RVL with int16_t can make the output 1.5 times
    // larger than its input. Given than byte is half the length of
    // int16_t, multiplying 3 (= 1.5 * 2) below.
    Bytes output(gsl::narrow<size_t>(input.size() * 3 / 2 * sizeof(T)));
    const int size{wilson::CompressRVL(const_cast<T*>(input.data()),
                                       reinterpret_cast<char*>(output.data()),
                                       gsl::narrow<int>(input.size()))};
    output.resize(size);
    output.shrink_to_fit();
    return output;
}

template <class T>
vector<T> decompress(const gsl::span<const std::byte> input, const int num_pixels) noexcept
{
    vector<T> output(num_pixels);
    wilson::DecompressRVL(
        reinterpret_cast<char*>(const_cast<std::byte*>(input.data())), output.data(), num_pixels);
    return output;
}
} // namespace rvl
} // namespace tg
