#include "loxobject.hpp"

#include "callable.hpp"
#include "interpreter.hpp"
#include <sstream>

#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-enum"

LoxObject::LoxObject(Callable* c, Interpreter* intp)
    : function{c}, interpreter{intp}, type{LoxType::Callable}
{
    interpreter->addUser(function);
}

LoxObject::LoxObject(LoxClass* lc, Interpreter* intp)
    : classy{lc}, interpreter{intp}, type{LoxType::Class}
{
    interpreter->addUser(classy);
}

LoxObject::LoxObject(LoxInstance* li, Interpreter* intp)
    : instance{li}, interpreter{intp}, type{LoxType::Instance}
{
    interpreter->addUser(instance);
}

LoxObject::LoxObject(const LoxObject& o)
    : string{o.string}, function{o.function}, classy{o.classy},
    instance{o.instance}, interpreter{o.interpreter}, number{o.number},
    type{o.type}, boolean{o.boolean}
{
    if (interpreter) registerCopy();
}

LoxObject& LoxObject::operator=(const LoxObject& o)
{
    string = o.string; function = o.function; classy = o.classy;
    instance = o.instance; interpreter = o.interpreter; number = o.number;
    type = o.type; boolean = o.boolean;
    if (interpreter) registerCopy();
    return *this;
}

LoxObject::~LoxObject()
{
    if (interpreter)
    {
        switch (type)
        {
        case LoxType::Callable:
            interpreter->removeUser(function);
            break;
        case LoxType::Class:
            interpreter->removeUser(classy);
            break;
        case LoxType::Instance:
            interpreter->removeUser(instance);
            break;
        default:
            throw std::logic_error("Lox object has bad type when destroyed.");
        }
    }

}

void LoxObject::registerCopy()
{
    switch (type)
    {
    case LoxType::Callable:
        interpreter->addUser(function);
        break;
    case LoxType::Class:
        interpreter->addUser(classy);
        break;
    case LoxType::Instance:
        interpreter->addUser(instance);
        break;
    default:
        throw std::logic_error("Expected object to be callable or class.");
    }
}

LoxObject::LoxObject(Token tok)
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

LoxObject LoxObject::operator()(Interpreter& intp,
                                std::vector<LoxObject> args)
{
    if (type == LoxType::Class)
    {
        if (args.size() != classy->arity())
        {
            std::string msg = "Function argument count mismatch. Expected "
                + std::to_string(classy->arity()) + ", got "
                + std::to_string(args.size()) + "\n";
            throw LoxError(msg);
        }
        return (*classy)(intp, args);
    }
    if (type != LoxType::Callable)
    {
        throw LoxError("Cannot call non-callable.");
    }
    if (args.size() != function->arity())
    {
        std::string msg = "Function argument count mismatch. Expected "
            + std::to_string(function->arity()) + ", got "
            + std::to_string(args.size()) + "\n";
        throw LoxError(msg);
    }
    return (*function)(intp, args);
}

LoxObject LoxObject::get(Token name)
{
    if (type != LoxType::Instance)
    {
        throw LoxError("Cannot get property from non-class instance.");
    }
    return instance->get(name);
}

LoxObject LoxObject::set(Token name, LoxObject value)
{
    if (type != LoxType::Instance)
    {
        throw LoxError("Cannot set property on non-class instance.");
    }
    return instance->set(name, value);
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
        ss.precision(15);
        ss << number;
        return ss.str();
    }
    case LoxType::String: return string;
    case LoxType::Callable:
        return "<fun " + function->name() + ">";
    case LoxType::Class:
        return "<class " + classy->name() + ">";
    case LoxType::Instance:
        return "<instance " + instance->name() + ">";
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
    case LoxType::Class:
    case LoxType::Instance:
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
    case LoxType::Callable: return true; // Callables and classes are
    case LoxType::Class: return true; // automatically true by definition.
    case LoxType::Instance: return true;
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
