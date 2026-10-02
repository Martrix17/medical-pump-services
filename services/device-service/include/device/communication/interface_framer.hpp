#ifndef INTERFACE_FRAMER_HPP
#define INTERFACE_FRAMER_HPP

#include <string>
#include <vector>

class IFramer {
  public:
    virtual ~IFramer() = default;

    virtual std::vector<std::string> process(std::string_view data) = 0;
    virtual void reset() = 0;
};

#endif  // INTERFACE_FRAMER_HPP