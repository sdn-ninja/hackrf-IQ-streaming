#ifndef IQLOOKUP_H
#define IQLOOKUP_H

#include <stdint.h>
#include <complex>

typedef std::complex<float> complex_t;

class iq_lookup
{
protected:
    complex_t _table[0xffff + 1];

public:
    iq_lookup()
    {
        const float scale = 1.0f / 128.0f;

        // create a lookup table for complex values
        for (uint32_t i = 0; i <= 0xffff; ++i)
        {
            float re = (float(i & 0xff) - 127.5f) * scale,
                  im = (float(i >> 8) - 127.5f) * scale;

            _table[i] = complex_t(re, im);
        }
    }

    const complex_t &lookup(uint16_t iq) const
    {
        return _table[iq];
    }
};

#endif
