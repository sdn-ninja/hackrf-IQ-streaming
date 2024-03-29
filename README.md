# blah2-hackrf
Fetching IQ data from 2 HackRFs
--
**Dependencies**

On Ubuntu, install the following:

    sudo apt install libhackrf-dev

You also need to define your devices' serial numbers in src/main.cpp (use hackrf_info to find SNs)

    char REF_DEVICE_SN[] = "FIRST_DEVICE_SERIAL_NUMBER";
    char SURV_DEVICE_SN[] = "SECOND_DEVICE_SERIAL_NUMBER";

Then

    mkdir build
    cd build
    cmake ..
    make
    ./blah2-hackrf

    
Kudos to evilsocket - Simone Margaritelli, author of HackRFPP
