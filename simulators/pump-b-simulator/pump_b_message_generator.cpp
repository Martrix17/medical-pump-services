#include "pump_b_message_generator.hpp"

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace {

constexpr std::size_t kFrameSize = 28;

constexpr std::uint8_t kHeaderFirst = 0xAA;
constexpr std::uint8_t kHeaderSecond = 0x55;

void appendUint16BE(std::string& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<char>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<char>(value & 0xFF));
}

void appendUint32BE(std::string& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<char>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<char>(value & 0xFF));
}

void appendUint64BE(std::string& buffer, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        buffer.push_back(static_cast<char>((value >> shift) & 0xFF));
    }
}

std::uint16_t calculateChecksum(std::string_view data) {
    std::uint16_t checksum = 0;
    for (const unsigned char byte : data) {
        checksum += byte;
    }
    return checksum;
}

}  // namespace

PumpBMessageGenerator::PumpBMessageGenerator(std::uint32_t deviceID)
    : deviceID_(deviceID), rng_(std::random_device{}()) {}

std::uint64_t PumpBMessageGenerator::randomTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();

    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

PumpBMessageGenerator::Status PumpBMessageGenerator::randomStatus() {
    static constexpr std::array statuses{
        Status::Disconnected, Status::Connecting, Status::Connected, Status::Error, Status::Unknown,
    };

    std::uniform_int_distribution<std::size_t> distribution(0, statuses.size() - 1);

    return statuses[distribution(rng_)];
}

PumpBMessageGenerator::MessageType PumpBMessageGenerator::randomMessageType() {
    static constexpr std::array messageTypes{
        MessageType::Status,
        MessageType::Measurement,
        MessageType::Alarm,
    };

    std::uniform_int_distribution<int> distribution(0, messageTypes.size() - 1);

    return messageTypes[distribution(rng_)];
}

std::optional<PumpBMessageGenerator::SimulatedMeasurement>
PumpBMessageGenerator::randomMeasurement() {
    std::bernoulli_distribution hasMeasurement(0.75);
    if (!hasMeasurement(rng_)) {
        return std::nullopt;
    }

    std::uniform_real_distribution<float> flow_rates(0.0, 5.0);
    std::uniform_real_distribution<float> pressures(0.0, 4.0);

    return SimulatedMeasurement{flow_rates(rng_), pressures(rng_)};
}

std::optional<PumpBMessageGenerator::SimulatedAlarm> PumpBMessageGenerator::randomAlarm() {
    std::bernoulli_distribution hasAlarm(0.5);
    if (!hasAlarm(rng_)) {
        return std::nullopt;
    }

    static constexpr std::array alarmTypes{
        AlarmType::Occlusion,
        AlarmType::LowPressure,
        AlarmType::DeviceFailure,
    };

    static constexpr std::array severities{
        Severity::Info,
        Severity::Warning,
        Severity::Critical,
    };

    std::uniform_int_distribution<std::size_t> typeDistribution(0, alarmTypes.size() - 1);

    std::uniform_int_distribution<std::size_t> severityDistribution(0, severities.size() - 1);

    return SimulatedAlarm{alarmTypes[typeDistribution(rng_)],
                          severities[severityDistribution(rng_)]};
}

std::string PumpBMessageGenerator::generate() {
    std::string buffer;
    buffer.reserve(kFrameSize);

    // Header
    buffer.push_back(static_cast<char>(kHeaderFirst));
    buffer.push_back(static_cast<char>(kHeaderSecond));

    // Type
    buffer.push_back(static_cast<char>(randomMessageType()));

    // Status
    buffer.push_back(static_cast<char>(randomStatus()));

    appendUint32BE(buffer, deviceID_);
    appendUint64BE(buffer, randomTimestamp());

    // Measurement
    if (const auto measurement = randomMeasurement(); measurement.has_value()) {
        const auto flowBits = std::bit_cast<std::uint32_t>(measurement->flowRate);
        const auto pressureBits = std::bit_cast<std::uint32_t>(measurement->pressure);
        appendUint32BE(buffer, flowBits);
        appendUint32BE(buffer, pressureBits);
    } else {
        appendUint32BE(buffer,
                       std::bit_cast<std::uint32_t>(std::numeric_limits<float>::quiet_NaN()));
        appendUint32BE(buffer,
                       std::bit_cast<std::uint32_t>(std::numeric_limits<float>::quiet_NaN()));
    }

    // Alarm
    if (const auto alarm = randomAlarm(); alarm.has_value()) {
        buffer.push_back(static_cast<char>(alarm->type));
        buffer.push_back(static_cast<char>(alarm->severity));
    } else {
        buffer.push_back(static_cast<char>(AlarmType::None));
        buffer.push_back(static_cast<char>(Severity::None));
    }

    // Checksum over bytes 0..25
    const auto checksum = calculateChecksum(buffer);
    appendUint16BE(buffer, checksum);

    if (buffer.size() != kFrameSize) {
        throw std::logic_error("Generated Pump B frame has invalid size");
    }

    // std::cout << "Generated Pump B frame: " << buffer << '\n';
    return buffer;
}