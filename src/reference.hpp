#ifndef REFERENCE_H
#define HACREFERENCE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <libhackrf/hackrf.h>

#include <stdexcept>
#include <vector>

#include "iqreader.hpp"

template <class REFERENCEDEMOD>
class ReferenceChannel
{
public:
    ReferenceChannel();
    virtual ~ReferenceChannel();

    void open(char dev_serial_number[]);

    void set_frequency(uint64_t freq);
    void set_sample_rate(uint64_t srate);
    void set_amp_enabled(bool enabled);
    void set_lna_gain(uint32_t gain);
    void set_vga_gain(uint32_t gain);
    void set_hw_sync_mode(uint8_t enabled);

    void start();
    void stop();

    bool is_streaming() const;

protected:
    hackrf_device *_referenceDevice;

    bool _running;
    iq_reader _iq_reader;
    REFERENCEDEMOD _demodulator;
    
    static int rx_callback(hackrf_transfer *transfer);

};


#define HACKRF_CHECK_STATUS(ret, message)  \
    if (ret != HACKRF_SUCCESS)             \
    {                                      \
        throw std::runtime_error(message); \
    }

template <class REFERENCEDEMOD>
ReferenceChannel<REFERENCEDEMOD>::ReferenceChannel() : _referenceDevice(NULL), _running(false)
{
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::open(char dev_serial_number[])
{
    int status = -1;

    status = hackrf_init();
    HACKRF_CHECK_STATUS(status, "Failed to initialize HackRf.");

    hackrf_device_list_t *list;
    list = hackrf_device_list();

    if (list->devicecount >= 2)
    {
        status = hackrf_open_by_serial(dev_serial_number, &_referenceDevice);
        HACKRF_CHECK_STATUS(status, "Failed to open device.");
    }
    else
    {
        std::cout << "Failed to find 2 attached HackRF devices" << std::endl;
    }
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_frequency(uint64_t freq)
{
    int status = hackrf_set_freq(_referenceDevice, freq);
    HACKRF_CHECK_STATUS(status, "Failed to set frequency.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_sample_rate(uint64_t srate)
{
    int status = hackrf_set_sample_rate(_referenceDevice, srate);
    HACKRF_CHECK_STATUS(status, "Failed to set sample rate.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_amp_enabled(bool enabled)
{
    int status = hackrf_set_amp_enable(_referenceDevice, enabled ? 1 : 0);
    HACKRF_CHECK_STATUS(status, "Failed to set AMP status.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_lna_gain(uint32_t gain)
{
    int status = hackrf_set_lna_gain(_referenceDevice, gain);
    HACKRF_CHECK_STATUS(status, "Failed to set LNA gain.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_vga_gain(uint32_t gain)
{
    int status = hackrf_set_vga_gain(_referenceDevice, gain);
    HACKRF_CHECK_STATUS(status, "Failed to set VGA gain.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::set_hw_sync_mode(uint8_t enabled)
{
    int status = hackrf_set_hw_sync_mode(_referenceDevice, enabled);
    HACKRF_CHECK_STATUS(status, "Failed to enable hardware synchronising.");
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::start()
{
    int status = hackrf_start_rx(_referenceDevice, rx_callback, this);
    HACKRF_CHECK_STATUS(status, "Failed to start RX streaming.");
    _running = true;
}

template <class REFERENCEDEMOD>
void ReferenceChannel<REFERENCEDEMOD>::stop()
{
    _running = false;
    hackrf_stop_rx(_referenceDevice);
}

template <class REFERENCEDEMOD>
bool ReferenceChannel<REFERENCEDEMOD>::is_streaming() const
{
    return (hackrf_is_streaming(_referenceDevice) == HACKRF_TRUE);
}

template <class REFERENCEDEMOD>
ReferenceChannel<REFERENCEDEMOD>::~ReferenceChannel()
{
    if (_referenceDevice)
    {
        stop();
        hackrf_close(_referenceDevice);
    }

    hackrf_exit();
}

template <class REFERENCEDEMOD>
int ReferenceChannel<REFERENCEDEMOD>::rx_callback(hackrf_transfer *transfer)
{
    ReferenceChannel *hrf = (ReferenceChannel *)transfer->rx_ctx;
    if (hrf->_running)
    {
        hrf->_demodulator.demodulate(hrf->_iq_reader.parse(transfer->buffer, transfer->valid_length), "Reference");
    }

    return 0;
}

#endif
