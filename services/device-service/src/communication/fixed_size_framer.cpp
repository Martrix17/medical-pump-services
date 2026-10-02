#include "device/communication/fixed_size_framer.hpp"

#include <stdexcept>

FixedSizeFramer::FixedSizeFramer(std::size_t frameSize) : frameSize_(frameSize) {
    if (frameSize_ == 0) {
        throw std::invalid_argument("FixedSizeFramer: Frame size must be greater than zero");
    }
}

std::vector<std::string> FixedSizeFramer::process(std::string_view data) {
    buffer_.append(data);

    std::vector<std::string> frames;

    while (buffer_.size() >= frameSize_) {
        frames.emplace_back(buffer_, 0, frameSize_);
        buffer_.erase(0, frameSize_);
    }

    return frames;
}

void FixedSizeFramer::reset() {
    buffer_.clear();
}
