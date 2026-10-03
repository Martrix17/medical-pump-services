#include "device/common/binary_reader.hpp"

#include <array>
#include <bit>
#include <stdexcept>

namespace {
void requireBytes(std::string_view data, std::size_t offset, std::size_t count) {
    if (offset > data.size() || data.size() - offset < count) {
        throw std::out_of_range("binary reader: not enough data");
    }
}

template <typename T> T readBigEndian(std::string_view data, std::size_t offset) {
    requireBytes(data, offset, sizeof(T));
    T value = 0;
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        value = static_cast<T>((value << 8) | static_cast<unsigned char>(data[offset + i]));
    }
    return value;
}

template <typename T> void appendBigEndian(std::string& buffer, T value) {
    std::array<char, sizeof(T)> bytes{};
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        const std::size_t shift = (sizeof(T) - 1 - i) * 8;
        bytes[i] = static_cast<char>((value >> shift) & 0xFF);
    }
    buffer.append(bytes.data(), bytes.size());
}

}  // namespace

std::uint8_t binary::readUint8(std::string_view data, std::size_t offset) {
    return readBigEndian<std::uint8_t>(data, offset);
}

std::uint16_t binary::readUint16(std::string_view data, std::size_t offset) {
    return readBigEndian<std::uint16_t>(data, offset);
}

std::uint32_t binary::readUint32(std::string_view data, std::size_t offset) {
    return readBigEndian<std::uint32_t>(data, offset);
}

std::uint64_t binary::readUint64(std::string_view data, std::size_t offset) {
    return readBigEndian<std::uint64_t>(data, offset);
}

float binary::readFloat32(std::string_view data, std::size_t offset) {
    static_assert(sizeof(float) == sizeof(std::uint32_t), "float must be 32 bits");
    return std::bit_cast<float>(readUint32(data, offset));
}

std::uint16_t binary::calculateChecksum(std::string_view data) {
    std::uint16_t checksum = 0;
    for (const unsigned char byte : data) {
        checksum = static_cast<std::uint16_t>(checksum + byte);
    }
    return checksum;
}

void binary::appendUint8(std::string& buffer, std::uint8_t value) {
    appendBigEndian<std::uint8_t>(buffer, value);
}

void binary::appendUint16(std::string& buffer, std::uint16_t value) {
    appendBigEndian<std::uint16_t>(buffer, value);
}

void binary::appendUint32(std::string& buffer, std::uint32_t value) {
    appendBigEndian<std::uint32_t>(buffer, value);
}

void binary::appendUint64(std::string& buffer, std::uint64_t value) {
    appendBigEndian<std::uint64_t>(buffer, value);
}

void binary::appendFloat32(std::string& buffer, float value) {
    static_assert(sizeof(float) == sizeof(std::uint32_t), "float must be 32 bits");
    appendUint32(buffer, std::bit_cast<std::uint32_t>(value));
}
