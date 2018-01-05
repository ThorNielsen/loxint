#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <iostream>
#include <string>
#include <stdexcept>

inline std::string error(size_t line, std::string msg)
{
    return "Error (" + std::to_string(line) + "): " + msg;
}

using LoxError = std::runtime_error;

#endif // ERROR_HPP_INCLUDED

