#ifndef _LOXOBJECT_HPP_INCLUDED
#define _LOXOBJECT_HPP_INCLUDED

#include <string>

enum class LoxType
{
    Nil, Bool, Number, String
};

class LoxObject
{
public:
    LoxObject(bool b)
        : string{}, number{}, type{LoxType::Bool}, boolean{b} {}
    LoxObject(double num)
        : string{}, number{num}, type{LoxType::Number}, boolean{} {}
    LoxObject(std::string s)
        : string{s}, number{}, type{LoxType::String}, boolean{} {}

    ~LoxObject() {}

    bool operator==(const LoxObject& o) const;

    std::string string;
    double number;
    LoxType type;
    bool boolean;
};

#endif // _LOXOBJECT_HPP_INCLUDED

