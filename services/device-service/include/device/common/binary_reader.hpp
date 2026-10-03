#ifndef BINARY_READER_HPP
#define BINARY_READER_HPP

#include <cstdint>
#include <string_view>

namespace binary {
std::uint8_t readUint8(std::string_view data, std::size_t offset);
std::uint16_t readUint16(std::string_view data, std::size_t offset);
std::uint32_t readUint32(std::string_view data, std::size_t offset);
std::uint64_t readUint64(std::string_view data, std::size_t offset);
float readFloat32(std::string_view data, std::size_t offset);

std::uint16_t calculateChecksum(std::string_view data);

void appendUint8(std::string& buffer, std::uint8_t value);
void appendUint16(std::string& buffer, std::uint16_t value);
void appendUint32(std::string& buffer, std::uint32_t value);
void appendUint64(std::string& buffer, std::uint64_t value);
void appendFloat32(std::string& buffer, float value);
}  // namespace binary

#endif  // BINARY_READER_HPP