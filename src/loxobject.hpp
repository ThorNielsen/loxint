#ifndef _LOXOBJECT_HPP_INCLUDED
#define _LOXOBJECT_HPP_INCLUDED

#include "token.hpp"
#include "error.hpp"

enum class LoxType
{
    Nil, Bool, Number, String
};

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

    bool operator==(const LoxObject& o) const;
    bool operator<(const LoxObject& o) const;
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
};

#endif // _LOXOBJECT_HPP_INCLUDED

