//
// Created by Hanseul Jun on 1/21/22.
//

#pragma once

#include "constants.hpp"

namespace tg
{
class PNGUtils
{
public:
    static Bytes write(int width,
                       int height,
                       const vector<uint8_t>& r_channel,
                       const vector<uint8_t>& g_channel,
                       const vector<uint8_t>& b_channel,
                       const vector<uint8_t>& a_channel);
};
} // namespace tg
