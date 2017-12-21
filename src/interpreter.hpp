#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "expr.hpp"
#include "loxobject.hpp"

class Interpreter : public ExprVisitor
{
public:
    Interpreter() {}
    ~Interpreter() {}

    LoxObject interpret(Expr* expr)
    {
        return expr->accept(*this);
    }

    ExprRetType visitBinaryExpr(Binary&) override;
    ExprRetType visitGroupingExpr(Grouping& grp) override
    {
        return grp.expr->accept(*this);
    }
    ExprRetType visitLiteralExpr(Literal& lit) override
    {
        return lit.value;
    }
    ExprRetType visitUnaryExpr(Unary&) override;
};

#endif // INTERPRETER_HPP_INCLUDED

