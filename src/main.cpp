#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <fstream>

#include "reference.hpp"
#include "survillance.hpp"
#include "bitstream.hpp"

#define FREQUENCY (225.648 * 1e6)
#define SAMPLE_RATE (8 * 1e6)
//Device serial numbers
char REF_DEVICE_SN[]  = "000000000000000042a068dc36413907";
char SURV_DEVICE_SN[] = "000000000000000057b068dc245abe63";

struct ByteEmitter
{
    static void emit(uint8_t byte, char const *source)
    {
        if (source == "Reference")
        {
            std::ofstream referenceChannelFile;
            referenceChannelFile.open("reference.bin", std::ios_base::app);
            referenceChannelFile << byte;
            referenceChannelFile.close();
        }
        else if (source == "Survillance")
        {
            std::ofstream survillanceChannelFile;
            survillanceChannelFile.open("survillance.bin", std::ios_base::app);
            survillanceChannelFile << byte;
            survillanceChannelFile.close();
        }
            /*
            std::cout << source << std::endl;
            printf("%02x ", byte);
            fflush(stdout);
            */
    }
};

struct AM
{
    static void demodulate(const std::vector<complex_t> &data, char const *signalSource)
    {

        bitstream<ByteEmitter> stream;
        stream.source = signalSource;
        double prev_mag = 0;

        for (std::vector<complex_t>::const_iterator i = data.begin(), e = data.end(); i != e; ++i)
        {
            const complex_t &c = *i;
            // scale magnitude in the interval [-1.0, ~1.0] ( 0.984406 )
            double magnitude = std::norm(c) - 1;
            //std::cout << c << std::endl;
            // manchester coding, low-to-high = 0, high-to-low = 1
            if (prev_mag < magnitude)
            {
                stream << 1;
            }
            else if (prev_mag > magnitude)
            {
                stream << 0;
            }
            //else
            //{
                //printf("\n");
            //}

            prev_mag = magnitude;
        }
    }
};

static bool stopped = false;

void signal_handler(int dummy)
{
    printf("\n@ Received CTRL+C\n");

    stopped = true;
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handler);

    std::cout << "Using HackRF device as input." << std::endl;

    try
    {
        ReferenceChannel<AM> referenceDev;
        SurvillanceChannel<AM> survillanceDev;

        referenceDev.open(REF_DEVICE_SN);
        referenceDev.set_frequency(FREQUENCY);
        referenceDev.set_sample_rate(SAMPLE_RATE);
        referenceDev.set_amp_enabled(false);
        referenceDev.set_lna_gain(32);
        referenceDev.set_vga_gain(30);

        survillanceDev.open(SURV_DEVICE_SN);
        survillanceDev.set_frequency(FREQUENCY);
        survillanceDev.set_sample_rate(SAMPLE_RATE);
        survillanceDev.set_amp_enabled(false);
        survillanceDev.set_lna_gain(32);
        survillanceDev.set_vga_gain(30);
        survillanceDev.set_hw_sync_mode(1); //Equivelant of -H hw_sync_enabl (in hackrf_transfer)] # Synchronise USB transfer using GPIO pins.
                                            //The surv device is the slave of the ref device in clock synchronization

        survillanceDev.start(); // start this first. it will wait for the ref device clock to trigger it
        referenceDev.start();

        while (stopped == false && referenceDev.is_streaming() && survillanceDev.is_streaming())
        {
            sleep(100);
        }

        referenceDev.stop();
        survillanceDev.stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
