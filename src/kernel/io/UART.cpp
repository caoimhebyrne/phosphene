#include "UART.h"
#include "MMIO.h"

namespace Kernel {

// 11.5: Register View
// https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#%5B%7B%22num%22%3A149%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C115%2C139.396%2Cnull%5D
struct Register {
    // NOTE: This does not include the peripheral base of 0x7C000000 (RPi 4)
    static const u32 Base = 0x201000;
    static const u32 Data = Base + 0x00;
    static const u32 Flag = Base + 0x18;
    static const u32 LineControl = Base + 0x2c;
    static const u32 Control = Base + 0x30;
};

// 11.5. Register View - FR Register
// https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#reg-UART-FR
struct Flag {
    static const u32 TransmitFIFOFull = 1 << 5;
    static const u32 ReceiveFIFOFull = 1 << 6;
};

// 11.5. Register View - LCRH Register
// https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#reg-UART-LCRH
struct LineControl {
    // These bits indicate the number of data bits transmitted or received in a frame
    struct WordLength {
        static const u32 Eight = 0b11 << 5;
    };

    static const u32 EnableFIFO = 1 << 4;
    static const u32 DisableFIFO = 0 << 4;
};

// 11.5. Register View - CR Register
// https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#reg-UART-CR
struct Control {
    static const u32 UARTEnable = 1 << 0;
    static const u32 TransmitEnable = 1 << 8;
    static const u32 ReceiveEnable = 1 << 9;
};

UART& UART::instance()
{
    static UART instance;
    return instance;
}

// See the "CR Register" section for instructions on programming the UART.
// https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf#reg-UART-CR
UART::UART()
{
    // 1. Disable the UART.
    MMIO::instance().write(Register::Control, 0);

    // 2. TODO: Wait for the end of transmission or reception of the current character.

    // 3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control Register
    MMIO::instance().write(Register::LineControl, LineControl::DisableFIFO);

    // - Enable FIFO and set the word length to 8
    //   TODO: Figure out why 8 is the only value that works, maybe I'm a dummy lol
    MMIO::instance().write(Register::LineControl, LineControl::EnableFIFO | LineControl::WordLength::Eight);

    // 4 + 5. Reprogram the control register + Enable the UART
    MMIO::instance().write(Register::Control, Control::UARTEnable | Control::ReceiveEnable | Control::TransmitEnable);
}

const char* u32_to_string(u32 value, char* output, u32 buffer_size)
{
    // 0 will always be 0.
    // We also have to special case this due to the while statement below...
    if (value == 0) {
        output[0] = '0';
        output[1] = '\0';

        return &output[1];
    }

    auto index = buffer_size - 1;
    output[index--] = 0;

    while (value > 0) {
        auto remainder = value % 10;
        output[index--] = '0' + remainder;

        value = value / 10;
    }

    // Because we are working from the end forward (index--), `index` will always point to
    // the byte before the last one that was written.
    return &output[index + 1];
}

void UART::print_raw(const char* string)
{
    for (auto i = 0; string[i] != '\0'; i++) {
        this->write(string[i]);
    }
}

void UART::print(const char* string, va_list arguments)
{
    // Loop over all characters in the string...
    for (auto i = 0; string[i] != '\0'; i++) {
        auto character = string[i];

        // If the character was an opening brace, we have started to parse an argument
        if (character == '{') {
            continue;
        }

        // If the last character was an opening brace, and the next character is a closing brace, do nothing
        if (string[i - 1] == '{' && string[i + 1] == '}') {
            continue;
        }

        // If the character is a closing brace, we have finished this argument, and can now print out its value
        if (character == '}') {
            auto type = string[i - 1];
            switch (type) {

            // Integer types (u32)
            case 'i': {
                auto value = va_arg(arguments, u32);

                char output[11];
                this->print_raw(u32_to_string(value, output, 11));

                break;
            }

            // Strings
            case 's': {
                auto value = va_arg(arguments, const char*);
                this->print_raw(value);

                break;
            }

            // Booleans (treated as int when parsing argument)
            case 'b': {
                auto value = va_arg(arguments, int);
                this->print_raw(value ? "true" : "false");

                break;
            }

            default:
                this->print_raw("{ Unsupported debug format type: '");
                this->write(type);
                this->print_raw("' }");

                break;
            }

            continue;
        }

        // The character was neither an opening, or a closing brace, so we can just write it normally
        this->write(character);
    }

    va_end(arguments);
}

void UART::print(const char* string, ...)
{
    va_list arguments;
    va_start(arguments, string);

    this->print(string, arguments);
}

void UART::println(const char* string, ...)
{
    va_list arguments;
    va_start(arguments, string);

    this->print(string, arguments);
    this->print("\r\n");
}

u32 UART::read()
{
    this->wait_until_ready_for_reading();

    return MMIO::instance().read(Register::Data);
}

void UART::write(u32 value)
{
    this->wait_until_ready_for_writing();

    MMIO::instance().write(Register::Data, value);
}

void UART::wait_until_ready_for_reading()
{
    // We need to wait until the receive FIFO is empty
    while (MMIO::instance().read(Register::Flag) & Flag::ReceiveFIFOFull) {
    }
}

void UART::wait_until_ready_for_writing()
{
    // We need to wait until the transmit FIFO is empty
    while (MMIO::instance().read(Register::Flag) & Flag::TransmitFIFOFull) {
    }
}
}