#include "loxobject.hpp"

#include "callable.hpp"
#include <sstream>

#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-enum"

LoxObject::LoxObject(Token tok)
    : string{}, function{}, number{}, type{}, boolean{}
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

LoxObject LoxObject::operator()(Interpreter& interpreter, std::vector<LoxObject> args)
{
    if (type != LoxType::Callable)
    {
        std::cout << (int)type << std::endl;
        throw LoxError("Cannot call non-callable.");
    }
    if (args.size() != function->arity())
    {
        std::string msg = "Function argument count mismatch. Expected "
            + std::to_string(function->arity()) + ", got "
            + std::to_string(args.size()) + "\n";
        throw LoxError(msg);
    }
    return (*function)(interpreter, args);
}


LoxObject::operator std::string() const
{
    switch (type)
    {
    case LoxType::Nil: return "nil";
    case LoxType::Bool: return boolean ? "true" : "false";
    case LoxType::Number:
    {
        std::stringstream ss;
        ss << number;
        return ss.str();
    }
    case LoxType::String: return string;
    case LoxType::Callable:
        // No conversion really makes sense. Should it be the function's name?
        // Its definition? A static string like "function"? Something else?
        // Make it an error to even try this as long as these questions hasn't
        // been answered.
        break;
    }
    throw LoxError("Could not convert object to string.");
}

LoxObject::operator double() const
{
    switch (type)
    {
    case LoxType::Nil: return 0.;
    case LoxType::Bool: return boolean ? 1. : 0.;
    case LoxType::Number: return number;
    case LoxType::String:
    {
        std::stringstream ss(string);
        double num;
        ss >> num;
        if (ss.fail() || ss.bad())
        {
            throw LoxError("Bad cast.");
        }
        return num;
    }
    case LoxType::Callable:
        // Yes, converting a function (or other callable) to a number makes
        // sense. Or not.
        break;
    }
    throw LoxError("Could not convert object to number.");
}

LoxObject::operator bool() const
{
    switch (type)
    {
    case LoxType::Nil:    return false;
    case LoxType::Bool:   return boolean;
    case LoxType::Number: return number != 0.;
    case LoxType::String: return string != "";
    case LoxType::Callable:
        // Is a function truthy? Or falsey? What about () -> false or
        // () -> true?
        break;
    }
    throw LoxError("Could not convert object to bool.");
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
        default:
            throw LoxError("Cannot compare objects for equality.");
        }
    }
    if (a.type > b.type) return b == a;
    if (a.type == LoxType::Nil || b.type == LoxType::Nil) return false;
    switch (a.type)
    {
    case LoxType::Bool: return a.boolean == (bool)b;
    case LoxType::Number: return a.number == (double)b;
    case LoxType::String: return a.string == (std::string)b;
    default:
        throw LoxError("Cannot compare objects for equality.");
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
        default:
            throw LoxError("Objects are not well-ordered.");
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
        default:
            throw LoxError("Cannot add objects.");
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
        default:
            throw LoxError("Cannot subtract objects.");
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
        default:
            throw LoxError("Cannot multiply objects.");
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
        default:
            throw LoxError("Cannot divide objects.");
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
    case LoxType::Number:
        a.number = -a.number;
        break;
    default:
        throw LoxError("Unary minus is not defined for current type.");
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
    default:
        throw LoxError("Cannot negate object.");
    }
    a.type = LoxType::Bool;
    return a;
}
