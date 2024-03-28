#ifndef SURVILLANCE_H
#define SURVILLANCE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <libhackrf/hackrf.h>

#include <stdexcept>
#include <vector>

#include "iqreader.hpp"

template <class SURVILLANCEDEMOD>
class SurvillanceChannel
{
public:
    SurvillanceChannel();
    virtual ~SurvillanceChannel();

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
    hackrf_device *_survillanceDevice;

    bool _running;
    iq_reader _iq_reader;
    SURVILLANCEDEMOD _demodulator;

    static int rx_callback(hackrf_transfer *transfer);
};

#define HACKRF_CHECK_STATUS(ret, message)  \
    if (ret != HACKRF_SUCCESS)             \
    {                                      \
        throw std::runtime_error(message); \
    }

template <class SURVILLANCEDEMOD>
SurvillanceChannel<SURVILLANCEDEMOD>::SurvillanceChannel() : _survillanceDevice(NULL), _running(false)
{
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::open(char dev_serial_number[])
{
    int status = -1;

    status = hackrf_init();
    HACKRF_CHECK_STATUS(status, "Failed to initialize HackRf.");

    hackrf_device_list_t *list;
    list = hackrf_device_list();

    if (list->devicecount >= 2)
    {
        status = hackrf_open_by_serial(dev_serial_number, &_survillanceDevice);
        HACKRF_CHECK_STATUS(status, "Failed to open device.");
    }
    else
    {
        std::cout << "Failed to find 2 attached HackRF devices" << std::endl;
    }
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_frequency(uint64_t freq)
{
    int status = hackrf_set_freq(_survillanceDevice, freq);
    HACKRF_CHECK_STATUS(status, "Failed to set frequency.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_sample_rate(uint64_t srate)
{
    int status = hackrf_set_sample_rate(_survillanceDevice, srate);
    HACKRF_CHECK_STATUS(status, "Failed to set sample rate.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_amp_enabled(bool enabled)
{
    int status = hackrf_set_amp_enable(_survillanceDevice, enabled ? 1 : 0);
    HACKRF_CHECK_STATUS(status, "Failed to set AMP status.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_lna_gain(uint32_t gain)
{
    int status = hackrf_set_lna_gain(_survillanceDevice, gain);
    HACKRF_CHECK_STATUS(status, "Failed to set LNA gain.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_vga_gain(uint32_t gain)
{
    int status = hackrf_set_vga_gain(_survillanceDevice, gain);
    HACKRF_CHECK_STATUS(status, "Failed to set VGA gain.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::set_hw_sync_mode(uint8_t enabled)
{
    int status = hackrf_set_hw_sync_mode(_survillanceDevice, enabled);
    HACKRF_CHECK_STATUS(status, "Failed to enable hardware synchronising.");
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::start()
{
    int status = hackrf_start_rx(_survillanceDevice, rx_callback, this);
    HACKRF_CHECK_STATUS(status, "Failed to start RX streaming.");
    _running = true;
}

template <class SURVILLANCEDEMOD>
void SurvillanceChannel<SURVILLANCEDEMOD>::stop()
{
    _running = false;
    hackrf_stop_rx(_survillanceDevice);
}

template <class SURVILLANCEDEMOD>
bool SurvillanceChannel<SURVILLANCEDEMOD>::is_streaming() const
{
    return (hackrf_is_streaming(_survillanceDevice) == HACKRF_TRUE);
}

template <class SURVILLANCEDEMOD>
SurvillanceChannel<SURVILLANCEDEMOD>::~SurvillanceChannel()
{
    if (_survillanceDevice)
    {
        stop();
        hackrf_close(_survillanceDevice);
    }

    hackrf_exit();
}

template <class SURVILLANCEDEMOD>
int SurvillanceChannel<SURVILLANCEDEMOD>::rx_callback(hackrf_transfer *transfer)
{
    SurvillanceChannel *hrf = (SurvillanceChannel *)transfer->rx_ctx;
    if (hrf->_running)
    {
        hrf->_demodulator.demodulate(hrf->_iq_reader.parse(transfer->buffer, transfer->valid_length), "Survillance");
    }

    return 0;
}

#endif
