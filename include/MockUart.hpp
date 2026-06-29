#ifndef MOCKUART_HPP
#define MOCKUART_HPP

#include "IUart.hpp"
#include "RingBuffer.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

class MockUart : public IUart {
public:
    static constexpr std::size_t DefaultRxCapacity = 128U;
    static constexpr std::size_t DefaultTxCapacity = 128U;

    MockUart();

    bool sendByte(std::uint8_t byte) override;
    bool receiveByte(std::uint8_t& byte) override;
    bool byteAvailable() const override;

    bool injectReceivedByte(std::uint8_t byte);
    std::size_t injectReceivedBytes(const std::uint8_t* data, std::size_t length);
    std::size_t injectReceivedString(const std::string& text);

    bool readTransmittedByte(std::uint8_t& byte);
    std::string readTransmittedString();

    std::size_t pendingRxBytes() const;
    std::size_t pendingTxBytes() const;
    void reset();

private:
    std::array<std::uint8_t, DefaultRxCapacity> rxStorage_{};
    std::array<std::uint8_t, DefaultTxCapacity> txStorage_{};
    RingBuffer rxBuffer_;
    RingBuffer txBuffer_;
};

#endif
