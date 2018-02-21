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
        : type{LoxType::Nil} {}
    LoxObject(bool b)
        : type{LoxType::Bool}, boolean{b} {}
    LoxObject(double num)
        : number{num}, type{LoxType::Number} {}
    LoxObject(std::string s)
        : string{s}, type{LoxType::String} {}
    LoxObject(Callable* c, Interpreter* intp);
    LoxObject(LoxClass* lc, Interpreter* intp);
    LoxObject(LoxInstance* li, Interpreter* intp);

    LoxObject(Token tok);

    LoxObject(const LoxObject&);
    LoxObject& operator=(const LoxObject&);

    ~LoxObject();

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

    std::string string = "";
    Callable* function = nullptr;
    LoxClass* classy = nullptr;
    LoxInstance* instance = nullptr;
    Interpreter* interpreter = nullptr;
    double number = 0.;
    LoxType type = LoxType::Nil;
    bool boolean = false;

private:
    void registerCopy();
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

