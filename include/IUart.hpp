#ifndef IUART_HPP
#define IUART_HPP

#include <cstdint>

class IUart {
public:
    virtual ~IUart() = default;

    virtual bool sendByte(std::uint8_t byte) = 0;
    virtual bool receiveByte(std::uint8_t& byte) = 0;
    virtual bool byteAvailable() const = 0;
};

#endif
