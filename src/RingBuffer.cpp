#include "RingBuffer.hpp"

RingBuffer::RingBuffer(std::uint8_t* storage, std::size_t capacity)
    : storage_(storage),
      capacity_(storage == nullptr ? 0U : capacity),
      head_(0U),
      tail_(0U),
      count_(0U) {}

bool RingBuffer::push(std::uint8_t byte) {
    if (full() || capacity_ == 0U) {
        return false;
    }

    storage_[head_] = byte;
    head_ = advance(head_);
    ++count_;
    return true;
}

bool RingBuffer::pop(std::uint8_t& byte) {
    if (empty()) {
        return false;
    }

    byte = storage_[tail_];
    tail_ = advance(tail_);
    --count_;
    return true;
}

bool RingBuffer::peek(std::uint8_t& byte) const {
    if (empty()) {
        return false;
    }

    byte = storage_[tail_];
    return true;
}

//pushes many bytes
std::size_t RingBuffer::write(const std::uint8_t* data, std::size_t length) {
    if (data == nullptr || length == 0U) {
        return 0U;
    }

    std::size_t written = 0U;
    while (written < length && push(data[written])) {
        ++written;
    }

    return written;
}

//pops many bytes
std::size_t RingBuffer::read(std::uint8_t* out, std::size_t length) {
    if (out == nullptr || length == 0U) {
        return 0U;
    }

    std::size_t bytesRead = 0U;
    while (bytesRead < length && pop(out[bytesRead])) {
        ++bytesRead;
    }

    return bytesRead;
}

void RingBuffer::reset() {
    head_ = 0U;
    tail_ = 0U;
    count_ = 0U;
}

bool RingBuffer::empty() const {
    return count_ == 0U;
}

bool RingBuffer::full() const {
    return count_ == capacity_ && capacity_ != 0U;
}

std::size_t RingBuffer::size() const {
    return count_;
}

std::size_t RingBuffer::capacity() const {
    return capacity_;
}

std::size_t RingBuffer::available() const {
    return capacity_ - count_;
}

std::size_t RingBuffer::advance(std::size_t index) const {
    ++index;
    if (index == capacity_) {
        index = 0U;
    }
    return index;
}
