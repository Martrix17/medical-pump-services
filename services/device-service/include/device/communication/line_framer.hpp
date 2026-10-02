#ifndef LINE_FRAMER_HPP
#define LINE_FRAMER_HPP

#include <string>
#include <vector>

#include "device/communication/interface_framer.hpp"

class LineFramer : public IFramer {
  public:
    explicit LineFramer(std::string delimiter = "\n");

    std::vector<std::string> process(std::string_view data) override;
    void reset() override;

  private:
    std::string delimiter_;
    std::string buffer_;
};

#endif  // LINE_FRAMER_HPP