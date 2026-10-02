#include "device/communication/line_framer.hpp"

LineFramer::LineFramer(std::string delimiter) : delimiter_(std::move(delimiter)) {}

std::vector<std::string> LineFramer::process(std::string_view data) {
    buffer_.append(data);

    std::vector<std::string> frames;
    std::size_t pos;

    while ((pos = buffer_.find(delimiter_)) != std::string::npos) {
        frames.emplace_back(buffer_, 0, pos);
        buffer_.erase(0, pos + delimiter_.size());
    }

    return frames;
}

void LineFramer::reset() {
    buffer_.clear();
}