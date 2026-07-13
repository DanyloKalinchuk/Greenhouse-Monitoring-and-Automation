#ifndef LINE_HPP
#define LINE_HPP

#include <cstdint>

class Line{
    public:
    virtual void write(uint32_t value) = 0;
    virtual bool read() = 0;
};

#endif