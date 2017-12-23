// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef STMT_HPP_INCLUDED
#define STMT_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>

using StmtRetType = LoxObject;

class Expression;
class Print;

class StmtVisitor
{
public:
    virtual LoxObject visitExpressionStmt(Expression&) = 0;
    virtual LoxObject visitPrintStmt(Print&) = 0;
};

class Stmt
{
public:
    virtual LoxObject accept(StmtVisitor&) = 0;
};

class Expression : public Stmt
{
public:
    Expression(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    LoxObject accept(StmtVisitor& v) override
    {
        return v.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

class Print : public Stmt
{
public:
    Print(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    LoxObject accept(StmtVisitor& v) override
    {
        return v.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

#endif // STMT_HPP_INCLUDED
