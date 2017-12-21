#ifndef _LOXOBJECT_HPP_INCLUDED
#define _LOXOBJECT_HPP_INCLUDED

#include "token.hpp"
#include "error.hpp"

enum class LoxType
{
    Nil = 0, Bool = 1, Number = 2, String = 3
};

inline bool operator<(LoxType a, LoxType b)
{
    return static_cast<int>(a) < static_cast<int>(b);
}

class LoxObject
{
public:
    LoxObject()
        : string{}, number{}, type{LoxType::Nil}, boolean{} {}
    LoxObject(bool b)
        : string{}, number{}, type{LoxType::Bool}, boolean{b} {}
    LoxObject(double num)
        : string{}, number{num}, type{LoxType::Number}, boolean{} {}
    LoxObject(std::string s)
        : string{s}, number{}, type{LoxType::String}, boolean{} {}

    LoxObject(Token tok);

    ~LoxObject() {}

    LoxObject& operator+=(const LoxObject& o);
    LoxObject& operator-=(const LoxObject& o);
    LoxObject& operator*=(const LoxObject& o);
    LoxObject& operator/=(const LoxObject& o);

    operator std::string() const;
    operator double() const;
    operator bool() const;

    std::string string;
    double number;
    LoxType type;
    bool boolean;
private:
    void cast(LoxType t)
    {
        switch (t)
        {
        case LoxType::Nil: break;
        case LoxType::Bool: boolean = (bool)(*this); break;
        case LoxType::Number: number = (double)(*this); break;
        case LoxType::String: string = (std::string)(*this); break;
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

