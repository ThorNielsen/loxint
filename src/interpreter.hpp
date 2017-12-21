#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "expr.hpp"

class Interpreter : public ExprVisitor
{
public:
    Interpreter() {}
    ~Interpreter() {}
/*
    ExprRetType visitBinaryExpr(Binary& bin) override
    {
        return
    }
    ExprRetType visitGroupingExpr(Grouping& grp) override
    {

    }*/
};

#endif // INTERPRETER_HPP_INCLUDED

