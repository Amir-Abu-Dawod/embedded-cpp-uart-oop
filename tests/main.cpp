#include "BufferedUart.hpp"
#include "MockUart.hpp"
#include "RingBuffer.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#define EXPECT_TRUE(condition)                                                        \
    do {                                                                               \
        if (!(condition)) {                                                            \
            std::cerr << "FAIL: " << __func__ << ": " << #condition                 \
                      << " at line " << __LINE__ << '\n';                            \
            return false;                                                              \
        }                                                                              \
    } while (false)

#define EXPECT_EQ(actual, expected)                                                    \
    do {                                                                               \
        const auto actualValue = (actual);                                             \
        const auto expectedValue = (expected);                                         \
        if (!(actualValue == expectedValue)) {                                         \
            std::cerr << "FAIL: " << __func__ << ": " << #actual                    \
                      << " != " << #expected << " at line " << __LINE__             \
                      << " actual=" << actualValue                                   \
                      << " expected=" << expectedValue << '\n';                      \
            return false;                                                              \
        }                                                                              \
    } while (false)

static bool testEmptyBuffer() {
    std::array<std::uint8_t, 4> storage{};
    RingBuffer buffer(storage.data(), storage.size());

    std::uint8_t byte = 0xAAU;
    EXPECT_TRUE(buffer.empty());
    EXPECT_TRUE(!buffer.full()); //! mark is a defensive embedded edge case.
    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_EQ(buffer.capacity(), 4U);
    EXPECT_TRUE(!buffer.pop(byte)); //! mark is a defensive embedded edge case.
    EXPECT_TRUE(!buffer.peek(byte)); //! mark is a defensive embedded edge case.

    return true;
}

static bool testFillAndOverflow() {
    std::array<std::uint8_t, 3> storage{};
    RingBuffer buffer(storage.data(), storage.size());

    EXPECT_TRUE(buffer.push('A'));
    EXPECT_TRUE(buffer.push('B'));
    EXPECT_TRUE(buffer.push('C'));
    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer.available(), 0U);
    EXPECT_TRUE(!buffer.push('D')); //! mark is a defensive embedded edge case.
    EXPECT_EQ(buffer.size(), 3U);

    return true;
}

static bool testFifoOrder() {
    std::array<std::uint8_t, 3> storage{};
    RingBuffer buffer(storage.data(), storage.size());

    EXPECT_TRUE(buffer.push(10U));
    EXPECT_TRUE(buffer.push(20U));
    EXPECT_TRUE(buffer.push(30U));

    std::uint8_t byte = 0U;
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 10U);
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 20U);
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 30U);
    EXPECT_TRUE(buffer.empty());

    return true;
}

static bool testWrapAround() {
    std::array<std::uint8_t, 3> storage{};
    RingBuffer buffer(storage.data(), storage.size());

    EXPECT_TRUE(buffer.push(1U));
    EXPECT_TRUE(buffer.push(2U));
    EXPECT_TRUE(buffer.push(3U));

    std::uint8_t byte = 0U;
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 1U);
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 2U);

    EXPECT_TRUE(buffer.push(4U));
    EXPECT_TRUE(buffer.push(5U));
    EXPECT_TRUE(buffer.full());

    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 3U);
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 4U);
    EXPECT_TRUE(buffer.pop(byte));
    EXPECT_EQ(byte, 5U);
    EXPECT_TRUE(buffer.empty());

    return true;
}

static bool testBulkReadWrite() {
    std::array<std::uint8_t, 5> storage{};
    RingBuffer buffer(storage.data(), storage.size());

    const std::uint8_t input[] = {'H', 'e', 'l', 'l', 'o', '!'};
    EXPECT_EQ(buffer.write(input, sizeof(input)), 5U);
    EXPECT_TRUE(buffer.full());

    std::uint8_t output[6]{};
    EXPECT_EQ(buffer.read(output, sizeof(output)), 5U);
    EXPECT_TRUE(std::memcmp(output, "Hello", 5U) == 0);
    EXPECT_TRUE(buffer.empty());

    return true;
}

static bool testZeroCapacitySafety() {
    RingBuffer buffer(nullptr, 0U);
    std::uint8_t byte = 0U;

    EXPECT_EQ(buffer.capacity(), 0U);
    EXPECT_TRUE(buffer.empty());
    EXPECT_TRUE(!buffer.full()); //! mark is a defensive embedded edge case.
    EXPECT_TRUE(!buffer.push(1U)); //! mark is a defensive embedded edge case.
    EXPECT_TRUE(!buffer.pop(byte)); //! mark is a defensive embedded edge case.

    return true;
}

static bool testMockUart() { //when later tests use BufferedUart, we can trust that MockUart itself is working.
    MockUart uart;

    EXPECT_EQ(uart.injectReceivedString("ABC"), 3U);
    EXPECT_TRUE(uart.byteAvailable());

    std::uint8_t byte = 0U;
    EXPECT_TRUE(uart.receiveByte(byte));
    EXPECT_EQ(byte, static_cast<std::uint8_t>('A'));
    EXPECT_TRUE(uart.receiveByte(byte));
    EXPECT_EQ(byte, static_cast<std::uint8_t>('B'));
    EXPECT_TRUE(uart.receiveByte(byte));
    EXPECT_EQ(byte, static_cast<std::uint8_t>('C'));
    EXPECT_TRUE(!uart.receiveByte(byte)); //! mark is a defensive embedded edge case.

    EXPECT_TRUE(uart.sendByte('O'));
    EXPECT_TRUE(uart.sendByte('K'));
    EXPECT_EQ(uart.readTransmittedString(), std::string("OK"));

    return true;
}

static bool testBufferedUartEchoFlow() { //if bytes arrive faster than the firmware consumes them, the RX software buffer can overflow.
    MockUart hardware;
    std::array<std::uint8_t, 8> rxStorage{};
    std::array<std::uint8_t, 8> txStorage{};
    BufferedUart uart(hardware, rxStorage.data(), rxStorage.size(), txStorage.data(), txStorage.size());

    EXPECT_EQ(hardware.injectReceivedString("Hello"), 5U);

    EXPECT_EQ(uart.pollRx(), 5U);
    EXPECT_EQ(uart.available(), 5U);

    EXPECT_EQ(uart.echoAvailable(), 5U);
    EXPECT_EQ(uart.available(), 0U);
    EXPECT_EQ(uart.queuedForTx(), 5U);

    EXPECT_EQ(uart.flushTx(), 5U);
    EXPECT_EQ(hardware.readTransmittedString(), std::string("Hello"));

    return true;
}

//If hardware TX cannot accept more bytes, BufferedUart stops flushing and keeps unsent bytes in its TX buffer.
static bool testBufferedRxOverflowDropsBytes() {
    MockUart hardware;
    std::array<std::uint8_t, 4> rxStorage{};
    std::array<std::uint8_t, 4> txStorage{};
    BufferedUart uart(hardware, rxStorage.data(), rxStorage.size(), txStorage.data(), txStorage.size());

    EXPECT_EQ(hardware.injectReceivedString("abcdef"), 6U);
    EXPECT_EQ(uart.pollRx(), 4U);
    EXPECT_EQ(uart.rxDropped(), 2U);
    EXPECT_EQ(uart.available(), 4U);

    std::uint8_t out[4]{};
    EXPECT_EQ(uart.read(out, sizeof(out)), 4U);
    EXPECT_TRUE(std::memcmp(out, "abcd", 4U) == 0);

    return true;
}

static bool testFlushTxStopsWhenHardwareTxFull() {
    MockUart hardware;
    std::array<std::uint8_t, 4> rxStorage{};
    std::array<std::uint8_t, 140> txStorage{};
    BufferedUart uart(hardware, rxStorage.data(), rxStorage.size(), txStorage.data(), txStorage.size());

    std::string longMessage(140, 'x');
    EXPECT_EQ(uart.write(reinterpret_cast<const std::uint8_t*>(longMessage.data()), longMessage.size()), 140U);

    EXPECT_EQ(uart.flushTx(), MockUart::DefaultTxCapacity);
    EXPECT_EQ(uart.queuedForTx(), 12U);
    EXPECT_EQ(hardware.pendingTxBytes(), MockUart::DefaultTxCapacity);

    return true; 
}

typedef bool (*TestFunction)();

int main() {
    const TestFunction tests[] = {
        testEmptyBuffer,
        testFillAndOverflow,
        testFifoOrder,
        testWrapAround,
        testBulkReadWrite,
        testZeroCapacitySafety,
        testMockUart,
        testBufferedUartEchoFlow,
        testBufferedRxOverflowDropsBytes,
        testFlushTxStopsWhenHardwareTxFull,
    };

    std::size_t passed = 0U;
    for (const auto test : tests) {
        if (!test()) {
            std::cerr << "Test failed. Passed " << passed << " tests before failure.\n";
            return 1;
        }
        ++passed;
    }

    std::cout << "All " << passed << " tests passed.\n";
    return 0;
}
