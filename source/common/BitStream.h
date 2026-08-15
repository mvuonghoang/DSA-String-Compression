#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <cstdint>

/**
 * @brief BitWriter enables writing data at bit-level granularity to an output stream.
 */
class BitWriter {
private:
    std::ostream& out;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitWriter(std::ostream& outputStream)
        : out(outputStream), buffer(0), bitCount(0) {}

    ~BitWriter() {
        flush();
    }

    /**
     * @brief Writes a single bit (0 or 1).
     */
    void writeBit(bool bit) {
        buffer = (buffer << 1) | (bit ? 1 : 0);
        bitCount++;
        if (bitCount == 8) {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }

    /**
     * @brief Writes 'numBits' from value (most significant bit first within the given bit width).
     */
    void writeBits(uint32_t value, int numBits) {
        for (int i = numBits - 1; i >= 0; --i) {
            writeBit((value >> i) & 1);
        }
    }

    /**
     * @brief Flushes any remaining bits in the buffer, padding with 0s to the right.
     */
    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
        out.flush();
    }
};

/**
 * @brief BitReader enables reading data at bit-level granularity from an input stream.
 */
class BitReader {
private:
    std::istream& in;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitReader(std::istream& inputStream)
        : in(inputStream), buffer(0), bitCount(0) {}

    /**
     * @brief Reads a single bit into the 'bit' reference.
     * @return true if a bit was successfully read, false on EOF.
     */
    bool readBit(bool& bit) {
        if (bitCount == 0) {
            char ch;
            if (!in.get(ch)) {
                return false;
            }
            buffer = static_cast<uint8_t>(ch);
            bitCount = 8;
        }
        bitCount--;
        bit = (buffer >> bitCount) & 1;
        return true;
    }

    /**
     * @brief Reads 'numBits' into the 'value' reference.
     * @return true if all 'numBits' were read, false if EOF encountered early.
     */
    bool readBits(uint32_t& value, int numBits) {
        value = 0;
        for (int i = 0; i < numBits; ++i) {
            bool bit;
            if (!readBit(bit)) {
                return false;
            }
            value = (value << 1) | (bit ? 1 : 0);
        }
        return true;
    }
};

#endif // BITSTREAM_H
