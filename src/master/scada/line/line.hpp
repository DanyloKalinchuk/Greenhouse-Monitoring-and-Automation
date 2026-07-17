#ifndef LINE_HPP
#define LINE_HPP

#include <cstdint>

/// \brief Base class for GPIO pin manipulation
class Line{
    public:
    virtual void write(uint32_t value) = 0;
    virtual bool read() = 0;
};

#endif