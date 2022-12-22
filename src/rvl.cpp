/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "rvl.hpp"

namespace rgbd
{
// Code inside this namespace is from the RVL paper (Wilson, 2017).
// The code has been modified to be thread-safe (i.e. removed global variables).
namespace wilson
{
void EncodeVLE(int64_t value, int64_t*& pBuffer, int64_t& word, int& nibblesWritten)
{
    do {
        int nibble = value & 0x7; // lower 3 bits
        if (value >>= 3)
            nibble |= 0x8; // more to come
        word <<= 4;
        word |= nibble;
//        if (++nibblesWritten == 8) { // output word
        if (++nibblesWritten == 16) { // output word
            *pBuffer++ = word;
            nibblesWritten = 0;
            word = 0;
        }
    } while (value);
}

int64_t DecodeVLE(int64_t*& pBuffer, int64_t& word, int& nibblesWritten)
{
    uint64_t nibble;
    int64_t value = 0;
//    int bits = 29;
    int bits = 61;
    do {
        if (!nibblesWritten) {
            word = *pBuffer++; // load word
//            nibblesWritten = 8;
            nibblesWritten = 16;
        }
        nibble = word & 0xf000000000000000;
        value |= (nibble << 1) >> bits;
        word <<= 4;
        nibblesWritten--;
        bits -= 3;
    } while (nibble & 0x8000000000000000);
    return value;
}
} // namespace wilson
} // namespace rgbd
