#include "device/common/binary_reader.hpp"

#include <bit>

std::uint8_t binary::readUint8(std::string_view data, std::size_t offset) {
    return static_cast<std::uint8_t>(static_cast<unsigned char>(data[offset]));
}

std::uint16_t binary::readUint16(std::string_view data, std::size_t offset) {
    return (static_cast<std::uint16_t>(static_cast<unsigned char>(data[offset])) << 8) |
           static_cast<std::uint16_t>(static_cast<unsigned char>(data[offset + 1]));
}

std::uint32_t binary::readUint32(std::string_view data, std::size_t offset) {
    return (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset])) << 24) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 1])) << 16) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 2])) << 8) |
           static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 3]));
}

std::uint64_t binary::readUint64(std::string_view data, std::size_t offset) {
    return (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset])) << 56) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 1])) << 48) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 2])) << 40) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 3])) << 32) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 4])) << 24) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 5])) << 16) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 6])) << 8) |
           static_cast<std::uint32_t>(static_cast<unsigned char>(data[offset + 7]));
}

float binary::readFloat32(std::string_view data, std::size_t offset) {
    const std::uint32_t bits = readUint32(data, offset);
    return std::bit_cast<float>(bits);
}

std::uint16_t binary::calculateChecksum(std::string_view data) {
    std::uint16_t checksum = 0;
    for (const unsigned char byte : data) {
        checksum += byte;
    }
    return checksum;
}
