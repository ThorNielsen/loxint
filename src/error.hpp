#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <iostream>
#include <string>
#include <stdexcept>

inline void error(size_t line, std::string msg)
{
    std::cerr << "Error (" << (line+1) << "): " << msg << "\n";
}

using LoxError = std::runtime_error;

#endif // ERROR_HPP_INCLUDED

