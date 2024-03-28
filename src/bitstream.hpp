#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>

template<class EMITTER>
class bitstream {
private:
    EMITTER _emitter;
    uint8_t _nbit;
    uint8_t _byte;

    void reset() {
        _nbit = 0;
        _byte = 0;
    }

public:

    char const *source;
    bitstream() {
        reset();
    }

    void collect_bit( uint8_t bit ) {
        _byte |= ( bit << _nbit );
        ++_nbit;
        if( _nbit >= 8 ) {
            _emitter.emit(_byte, source);
            reset();
        }
    }
};

template<class EMITTER>
bitstream<EMITTER>& operator<<(bitstream<EMITTER>& bs, const uint8_t& bit ) {
    bs.collect_bit(bit);
    return bs;
}

#endif
