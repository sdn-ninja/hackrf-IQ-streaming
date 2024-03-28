#ifndef IQREADER_H
#define IQREADER_H

#include <vector>

#include "iqlookup.hpp"

class iq_reader
{
protected:
    iq_lookup _cache;

public:
    std::vector<complex_t> parse(const uint8_t *buffer, uint32_t size) const
    {
        // The HackRF will stream to us IQ values of two bytes.
        uint16_t *p = (uint16_t *)buffer;
        size_t i, len = size / sizeof(uint16_t);

        std::vector<complex_t> values(len);
        for (i = 0; i < len; ++i)
        {
            values.push_back(_cache.lookup(p[i]));
        }

        return values;
    }
};

#endif
