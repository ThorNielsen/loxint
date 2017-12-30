#ifndef _LOXOBJECT_HPP_INCLUDED
#define _LOXOBJECT_HPP_INCLUDED

#include "token.hpp"
#include "error.hpp"
#include <memory>
#include <vector>

enum class LoxType
{
    Nil = 0, Bool = 1, Number = 2, String = 3, Callable = 4,
};

inline bool operator<(LoxType a, LoxType b)
{
    return static_cast<int>(a) < static_cast<int>(b);
}

class Interpreter;
class Callable;

using LoxCallable = std::shared_ptr<Callable>;

class LoxObject
{
public:
    LoxObject()
        : string{}, function{}, number{}, type{LoxType::Nil}, boolean{} {}
    LoxObject(bool b)
        : string{}, function{}, number{}, type{LoxType::Bool}, boolean{b} {}
    LoxObject(double num)
        : string{}, function{}, number{num}, type{LoxType::Number}, boolean{} {}
    LoxObject(std::string s)
        : string{s}, function{}, number{}, type{LoxType::String}, boolean{} {}
    LoxObject(LoxCallable c)
        : string{}, function{c}, number{}, type{LoxType::Callable}, boolean{} {}

    LoxObject(Token tok);

    ~LoxObject() {}

    LoxObject operator()(Interpreter&, std::vector<LoxObject> args);

    LoxObject& operator+=(const LoxObject& o);
    LoxObject& operator-=(const LoxObject& o);
    LoxObject& operator*=(const LoxObject& o);
    LoxObject& operator/=(const LoxObject& o);

    operator std::string() const;
    operator double() const;
    operator bool() const;

    std::string string;
    LoxCallable function;
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
        switch (t)
        {
        case LoxType::Nil: break;
        case LoxType::Bool: boolean = (bool)(*this); break;
        case LoxType::Number: number = (double)(*this); break;
        case LoxType::String: string = (std::string)(*this); break;
        case LoxType::Callable:
            throw LoxError("Cannot convert non-callable to callable.");
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

