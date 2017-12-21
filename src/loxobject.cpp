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
    case LoxType::Number: return std::to_string(number);
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
        double d;
        ss >> d;
        if (ss.fail() || ss.bad())
        {
            throw LoxError("Bad cast.");
        }
        return d;
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
    case LoxType::String: return (string != "false") && (string != "");
    }
    return false;
}

bool LoxObject::operator==(const LoxObject& o) const
{
    if (type == o.type)
    {
        switch (type)
        {
        case LoxType::Nil:
            return true;
        case LoxType::Bool:
            return boolean == o.boolean;
        case LoxType::Number:
            return number == o.number;
        case LoxType::String:
            return string == o.string;
        }
    }
    if (type == LoxType::Nil || o.type == LoxType::Nil) return false;
    switch (type)
    {
    case LoxType::Bool:
        switch(o.type)
        {
            case LoxType::Number: return boolean == (o.number != 0);
            case LoxType::String: return boolean == (o.string != "");
        }
    case LoxType::Number:
        switch(o.type)
        {
            case LoxType::Bool: return (number != 0) == o.boolean;
            case LoxType::String: return std::to_string(number) == o.string;
        }
    case LoxType::String:
        switch(o.type)
        {
            case LoxType::Bool: return (string != "") == o.boolean;
            case LoxType::Number: return string == std::to_string(o.number);
        }
    }
    return false;
}
