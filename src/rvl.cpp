/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "rvl.hpp"

namespace rgbd
{
// Code inside this namespace is from the RVL paper (Wilson, 2017).
// The code has been modified to be thread-safe (i.e. removed global variables).
// And the code has been modified to support signed 32-bit integers as input of RVL.
namespace wilson
{
void EncodeVLE(int64_t value, int*& pBuffer, int& word, int& nibblesWritten)
{
    do {
        int nibble = value & 0x7; // lower 3 bits
        if (value >>= 3)
            nibble |= 0x8; // more to come
        word <<= 4;
        word |= nibble;
        if (++nibblesWritten == 8) { // output word
            *pBuffer++ = word;
            nibblesWritten = 0;
            word = 0;
        }
    } while (value);
}

int64_t DecodeVLE(int*& pBuffer, int& word, int& nibblesWritten)
{
//    unsigned int nibble;
    uint64_t nibble;
    int64_t value = 0;
//    int bits = 29;
    int bits = 61;
    do {
        if (!nibblesWritten) {
            word = *pBuffer++; // load word
            nibblesWritten = 8;
        }
        nibble = word & 0xf0000000;
//        value |= (nibble << 1) >> bits;
        value |= (nibble << 33) >> bits;
        word <<= 4;
        --nibblesWritten;
        bits -= 3;
    } while (nibble & 0x80000000);
    return value;
}
} // namespace wilson
} // namespace rgbd
