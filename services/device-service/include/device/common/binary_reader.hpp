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
}  // namespace binary

#endif  // BINARY_READER_HPP