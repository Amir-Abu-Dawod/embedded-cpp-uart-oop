#include "MockUart.hpp"

MockUart::MockUart()
    : rxBuffer_(rxStorage_.data(), rxStorage_.size()),
      txBuffer_(txStorage_.data(), txStorage_.size()) {}

bool MockUart::sendByte(std::uint8_t byte) {
    return txBuffer_.push(byte);
}

bool MockUart::receiveByte(std::uint8_t& byte) {
    return rxBuffer_.pop(byte);
}

bool MockUart::byteAvailable() const {
    return !rxBuffer_.empty();
}

bool MockUart::injectReceivedByte(std::uint8_t byte) {
    return rxBuffer_.push(byte);
}

std::size_t MockUart::injectReceivedBytes(const std::uint8_t* data, std::size_t length) {
    return rxBuffer_.write(data, length);
}

//UART does not really transmit “strings.” UART transmits bytes. A string is just a convenient way to provide test bytes.
std::size_t MockUart::injectReceivedString(const std::string& text) {
    return injectReceivedBytes(reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
}

bool MockUart::readTransmittedByte(std::uint8_t& byte) {
    return txBuffer_.pop(byte);
}

std::string MockUart::readTransmittedString() {
    std::string result;
    result.reserve(txBuffer_.size());

    std::uint8_t byte = 0U;
    while (readTransmittedByte(byte)) {
        result.push_back(static_cast<char>(byte));
    }

    return result;
}

std::size_t MockUart::pendingRxBytes() const {
    return rxBuffer_.size();
}

std::size_t MockUart::pendingTxBytes() const {
    return txBuffer_.size();
}

void MockUart::reset() {
    rxBuffer_.reset();
    txBuffer_.reset();
}
