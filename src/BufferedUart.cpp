#include "BufferedUart.hpp"

BufferedUart::BufferedUart(IUart& uart,
                           std::uint8_t* rxStorage,
                           std::size_t rxCapacity,
                           std::uint8_t* txStorage,
                           std::size_t txCapacity)
    : uart_(uart),
      rxBuffer_(rxStorage, rxCapacity),
      txBuffer_(txStorage, txCapacity),
      rxDropped_(0U) {}

std::size_t BufferedUart::pollRx() {
    std::size_t copied = 0U;
    std::uint8_t byte = 0U;

    while (uart_.byteAvailable() && uart_.receiveByte(byte)) {
        if (rxBuffer_.push(byte)) {
            ++copied;
        } else {
            //If the RX software buffer is full:
            ++rxDropped_;
        }
    }

    return copied;
}

std::size_t BufferedUart::flushTx() {
    std::size_t flushed = 0U;
    std::uint8_t byte = 0U;
    
    //The use of peek() before pop() prevents data loss when the destination is full.
    while (txBuffer_.peek(byte)) {
        if (!uart_.sendByte(byte)) {
            break;
        }
        //Only remove from TX buffer after hardware accepted the byte. This protects against losing data
        txBuffer_.pop(byte);
        ++flushed;
    }

    return flushed;
}

//It only prepares the bytes for transmission. Then flushTx() actually sends them.
std::size_t BufferedUart::echoAvailable() {
    std::size_t echoed = 0U;
    std::uint8_t byte = 0U;

    while (rxBuffer_.peek(byte)) {
        if (!txBuffer_.push(byte)) {
            break;
        }

        rxBuffer_.pop(byte);
        ++echoed;
    }

    return echoed;
}

bool BufferedUart::readByte(std::uint8_t& byte) {
    return rxBuffer_.pop(byte);
}

bool BufferedUart::writeByte(std::uint8_t byte) {
    return txBuffer_.push(byte);
}

std::size_t BufferedUart::read(std::uint8_t* out, std::size_t length) {
    return rxBuffer_.read(out, length);
}

std::size_t BufferedUart::write(const std::uint8_t* data, std::size_t length) {
    return txBuffer_.write(data, length);
}

std::size_t BufferedUart::available() const {
    return rxBuffer_.size();
}

std::size_t BufferedUart::queuedForTx() const {
    return txBuffer_.size();
}

std::size_t BufferedUart::rxFree() const {
    return rxBuffer_.available();
}

std::size_t BufferedUart::txFree() const {
    return txBuffer_.available();
}

std::size_t BufferedUart::rxDropped() const {
    return rxDropped_;
}

void BufferedUart::resetBuffers() {
    rxBuffer_.reset();
    txBuffer_.reset();
    rxDropped_ = 0U;
}
