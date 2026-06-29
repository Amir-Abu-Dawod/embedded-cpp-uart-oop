#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <cstddef>
#include <cstdint>

class RingBuffer {
public:
    RingBuffer(std::uint8_t* storage, std::size_t capacity);

    bool push(std::uint8_t byte);
    bool pop(std::uint8_t& byte);
    bool peek(std::uint8_t& byte) const;

    std::size_t write(const std::uint8_t* data, std::size_t length);
    std::size_t read(std::uint8_t* out, std::size_t length);

    void reset();

    bool empty() const;
    bool full() const;
    std::size_t size() const;
    std::size_t capacity() const;
    std::size_t available() const;

private:
    std::uint8_t* storage_;
    std::size_t capacity_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;

    std::size_t advance(std::size_t index) const;
};

#endif
