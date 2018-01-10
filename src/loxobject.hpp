#ifndef _LOXOBJECT_HPP_INCLUDED
#define _LOXOBJECT_HPP_INCLUDED

#include "token.hpp"
#include "error.hpp"
#include <memory>
#include <vector>

enum class LoxType
{
    Nil = 0, Bool = 1, Number = 2, String = 3, Callable = 4, Class = 5,
    Instance = 6,
};

inline bool operator<(LoxType a, LoxType b)
{
    return static_cast<int>(a) < static_cast<int>(b);
}

class Interpreter;
class Callable;
class LoxClass;
class LoxInstance;

class LoxObject
{
public:
    LoxObject()
        : string{}, function{}, classy{}, instance{}, number{}, type{LoxType::Nil}, boolean{} {}
    LoxObject(bool b)
        : string{}, function{}, classy{}, instance{}, number{}, type{LoxType::Bool}, boolean{b} {}
    LoxObject(double num)
        : string{}, function{}, classy{}, instance{}, number{num}, type{LoxType::Number}, boolean{} {}
    LoxObject(std::string s)
        : string{s}, function{}, classy{}, instance{}, number{}, type{LoxType::String}, boolean{} {}
    LoxObject(std::shared_ptr<Callable> c)
        : string{}, function{c}, classy{}, instance{}, number{}, type{LoxType::Callable}, boolean{} {}
    LoxObject(std::shared_ptr<LoxClass> c)
        : string{}, function{}, classy{c}, instance{}, number{}, type{LoxType::Class}, boolean{} {}
    LoxObject(std::shared_ptr<LoxInstance> li)
        : string{}, function{}, classy{}, instance{li}, number{}, type{LoxType::Instance}, boolean{} {}

    LoxObject(Token tok);

    ~LoxObject() {}

    LoxObject operator()(Interpreter&, std::vector<LoxObject> args);

    LoxObject& operator+=(const LoxObject& o);
    LoxObject& operator-=(const LoxObject& o);
    LoxObject& operator*=(const LoxObject& o);
    LoxObject& operator/=(const LoxObject& o);

    LoxObject get(Token name);
    LoxObject set(Token name, LoxObject value);

    operator std::string() const;
    operator double() const;
    operator bool() const;

    std::string string;
    std::shared_ptr<Callable> function;
    std::shared_ptr<LoxClass> classy;
    std::shared_ptr<LoxInstance> instance;
    double number;
    LoxType type;
    bool boolean;
private:
    void cast(LoxType t)
    {
        if (t == type) return;
        if (type == LoxType::Callable)
        {
            throw LoxError("Cannot convert callable to non-callable.");
        }
        if (type == LoxType::Class || type == LoxType::Instance)
        {
            throw LoxError("Cannot convert class to non-class.");
        }
        switch (t)
        {
        case LoxType::Nil: break;
        case LoxType::Bool: boolean = (bool)(*this); break;
        case LoxType::Number: number = (double)(*this); break;
        case LoxType::String: string = (std::string)(*this); break;
        case LoxType::Callable:
            throw LoxError("Cannot convert non-callable to callable.");
        case LoxType::Class:
        case LoxType::Instance:
            throw LoxError("Cannot convert non-class to class.");
        }
        type = t;
    }
};

LoxObject operator-(LoxObject a);
LoxObject operator!(LoxObject a);

bool operator==(const LoxObject& a, const LoxObject& b);
bool operator<(const LoxObject& a, const LoxObject& b);

inline bool operator!=(const LoxObject& a, const LoxObject& b)
{
    return !(a == b);
}

inline bool operator<=(const LoxObject& a, const LoxObject& b)
{
    return a < b || a == b;
}

inline bool operator>(const LoxObject& a, const LoxObject& b)
{
    return !(a <= b);
}

inline bool operator>=(const LoxObject& a, const LoxObject& b)
{
    return !(a < b);
}

inline LoxObject operator+(LoxObject a, const LoxObject& b)
{
    return a += b;
}
inline LoxObject operator-(LoxObject a, const LoxObject& b)
{
    return a -= b;
}
inline LoxObject operator*(LoxObject a, const LoxObject& b)
{
    return a *= b;
}
inline LoxObject operator/(LoxObject a, const LoxObject& b)
{
    return a /= b;
}

#endif // _LOXOBJECT_HPP_INCLUDED

