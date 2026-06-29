#ifndef BUFFEREDUART_HPP
#define BUFFEREDUART_HPP

#include "IUart.hpp"
#include "RingBuffer.hpp"

#include <cstddef>
#include <cstdint>

class BufferedUart {
public:
    BufferedUart(IUart& uart,
                 std::uint8_t* rxStorage,
                 std::size_t rxCapacity,
                 std::uint8_t* txStorage,
                 std::size_t txCapacity);

    std::size_t pollRx();
    std::size_t flushTx();
    std::size_t echoAvailable();

    bool readByte(std::uint8_t& byte);
    bool writeByte(std::uint8_t byte);

    std::size_t read(std::uint8_t* out, std::size_t length);
    std::size_t write(const std::uint8_t* data, std::size_t length);

    std::size_t available() const;
    std::size_t queuedForTx() const;
    std::size_t rxFree() const;
    std::size_t txFree() const;
    std::size_t rxDropped() const;

    void resetBuffers();

private:
    IUart& uart_;
    RingBuffer rxBuffer_;
    RingBuffer txBuffer_;
    std::size_t rxDropped_;
};

#endif
