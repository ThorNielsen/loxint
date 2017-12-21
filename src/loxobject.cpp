#include "loxobject.hpp"


#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch"

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
