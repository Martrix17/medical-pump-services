#ifndef INTERFACE_MESSAGE_GENERATOR_HPP
#define INTERFACE_MESSAGE_GENERATOR_HPP

#include <string>

class IMessageGenerator {
  public:
    virtual ~IMessageGenerator() = default;

    virtual std::string generate() = 0;
};

#endif  // INTERFACE_MESSAGE_GENERATOR_HPP