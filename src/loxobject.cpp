#include "loxobject.hpp"

#include <sstream>

#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wswitch-enum"

LoxObject::LoxObject(Token tok)
    : string{}, number{}, type{}, boolean{}
{
    switch (tok.type)
    {
    case TokenType::Nil:
        type = LoxType::Nil;
        break;
    case TokenType::True:
        type = LoxType::Bool;
        boolean = true;
        break;
    case TokenType::False:
        type = LoxType::Bool;
        boolean = false;
        break;
    case TokenType::Number:
        type = LoxType::Number;
        // Limit scope of stringstream.
        {
            std::stringstream ss(tok.lexeme);
            ss >> number;
        }
        break;
    case TokenType::String:
        type = LoxType::String;
        string = tok.lexeme;
        break;
    default:
        throw LoxError("Invalid Lox object.");
    }
}

LoxObject::operator std::string() const
{
    switch (type)
    {
    case LoxType::Nil: return "";
    case LoxType::Bool: return boolean ? "true" : "false";
    case LoxType::Number:
    {
        std::stringstream ss;
        ss << number;
        return ss.str();
    }
    case LoxType::String: return string;
    }
    return "impossible";
}

LoxObject::operator double() const
{
    switch (type)
    {
    case LoxType::Nil: return 0.;
    case LoxType::Bool: return boolean ? 1. : 0.;
    case LoxType::Number: return number;
    case LoxType::String:
        std::stringstream ss(string);
        double num;
        ss >> num;
        if (ss.fail() || ss.bad())
        {
            throw LoxError("Bad cast.");
        }
        return num;
    }
    return 69105.;
}

LoxObject::operator bool() const
{
    switch (type)
    {
    case LoxType::Nil: return false;
    case LoxType::Bool: return boolean;
    case LoxType::Number: return number != 0.;
    case LoxType::String: return string != "";
    }
    return false;
}

bool operator==(const LoxObject& a, const LoxObject& b)
{
    if (a.type == b.type)
    {
        switch (a.type)
        {
        case LoxType::Nil:
            return true;
        case LoxType::Bool:
            return a.boolean == b.boolean;
        case LoxType::Number:
            return a.number == b.number;
        case LoxType::String:
            return a.string == b.string;
        }
    }
    if (a.type > b.type) return b == a;
    if (a.type == LoxType::Nil || b.type == LoxType::Nil) return false;
    switch (a.type)
    {
    case LoxType::Bool: return a.boolean == (bool)b;
    case LoxType::Number: return a.number == (double)b;
    case LoxType::String: return a.string == (std::string)b;
    }
    return false;
}

bool operator<(const LoxObject& a, const LoxObject& b)
{
    if (a.type == b.type)
    {
        switch (a.type)
        {
        case LoxType::Nil: case LoxType::Bool:
            throw LoxError("Nil and bool are not well-ordered.");
        case LoxType::Number:
            return a.number < b.number;
        case LoxType::String:
            return a.string < b.string;
        }
    }
    throw LoxError("Only objects of the same type can be well-ordered.");
}

LoxObject& LoxObject::operator+=(const LoxObject& o)
{
    if (type == o.type)
    {
        switch (type)
        {
        case LoxType::Nil:
            throw LoxError("Cannot add nil.");
        case LoxType::Bool:
            throw LoxError("Cannot add bools.");
        case LoxType::Number:
            number += o.number;
            break;
        case LoxType::String:
            string += o.string;
            break;
        }
        return *this;
    }
    if (type < o.type)
    {
        cast(o.type);
        return (*this) += o;
    }
    else
    {
        auto b = o;
        b.cast(type);
        return (*this) += b;
    }
}

LoxObject& LoxObject::operator-=(const LoxObject& o)
{
    if (type == o.type)
    {
        switch (type)
        {
        case LoxType::Nil:
            throw LoxError("Cannot subtract nil.");
        case LoxType::Bool:
            throw LoxError("Cannot subtract bools.");
        case LoxType::Number:
            number -= o.number;
            break;
        case LoxType::String:
            throw LoxError("Cannot subtract string.");
        }
        return *this;
    }
    if (type < o.type)
    {
        cast(o.type);
        return (*this) -= o;
    }
    else
    {
        auto b = o;
        b.cast(type);
        return (*this) -= b;
    }
}

LoxObject& LoxObject::operator*=(const LoxObject& o)
{
    if (type == o.type)
    {
        switch (type)
        {
        case LoxType::Nil:
            throw LoxError("Cannot multiply nil.");
        case LoxType::Bool:
            throw LoxError("Cannot multiply bools.");
        case LoxType::Number:
            number *= o.number;
            break;
        case LoxType::String:
            throw LoxError("Cannot multiply strings.");
        }
        return *this;
    }
    if (type < o.type)
    {
        cast(o.type);
        return (*this) *= o;
    }
    else
    {
        auto b = o;
        b.cast(type);
        return (*this) *= b;
    }
}

LoxObject& LoxObject::operator/=(const LoxObject& o)
{
    if (type == o.type)
    {
        switch (type)
        {
        case LoxType::Nil:
            throw LoxError("Cannot divide nil.");
        case LoxType::Bool:
            throw LoxError("Cannot divide bools.");
        case LoxType::Number:
            number /= o.number;
            break;
        case LoxType::String:
            throw LoxError("Cannot divide strings.");
        }
        return *this;
    }
    if (type < o.type)
    {
        cast(o.type);
        return (*this) /= o;
    }
    else
    {
        auto b = o;
        b.cast(type);
        return (*this) /= b;
    }
}

LoxObject operator-(LoxObject a)
{
    switch (a.type)
    {
    case LoxType::Nil: case LoxType::Bool: case LoxType::String:
        throw LoxError("Unary minus is not defined for current type.");
    case LoxType::Number:
        a.number = -a.number;
    }
    return a;
}

LoxObject operator!(LoxObject a)
{
    switch (a.type)
    {
    case LoxType::Nil:
        a.boolean = true;
        break;
    case LoxType::Number:
        a.boolean = a.number == 0.;
        break;
    case LoxType::String:
        a.boolean = a.string == "";
        break;
    case LoxType::Bool:
        a.boolean = !a.boolean;
        break;
    }
    a.type = LoxType::Bool;
    return a;
}
