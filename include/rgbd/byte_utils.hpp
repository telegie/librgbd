/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "constants.hpp"

namespace tg
{
template <class T> Bytes convert_to_bytes(const T& t)
{
    Bytes bytes(sizeof(T));
    memcpy(&bytes[0], &t, sizeof(T));
    return bytes;
}

void append_bytes(Bytes& bytes1, const Bytes& bytes2);

template <class T> T read_from_bytes(gsl::span<const std::byte> bytes, int& cursor)
{
    T t;
    memcpy(&t, &bytes[cursor], sizeof(T));
    cursor += sizeof(T);
    return t;
}
} // namespace tg
