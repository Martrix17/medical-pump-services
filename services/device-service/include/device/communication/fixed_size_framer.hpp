#ifndef FIXED_SIZE_FRAMER_HPP
#define FIXED_SIZE_FRAMER_HPP

#include <string>
#include <vector>

#include "device/communication/interface_framer.hpp"

class FixedSizeFramer : public IFramer {
  public:
    explicit FixedSizeFramer(std::size_t frameSize = 28);

    std::vector<std::string> process(std::string_view data) override;
    void reset() override;

  private:
    std::size_t frameSize_;
    std::string buffer_;
};

#endif  // FIXED_SIZE_FRAMER_HPP