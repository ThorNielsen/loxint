// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef STMT_HPP_INCLUDED
#define STMT_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>

using StmtRetType = void;

class ExpressionStmt;
class PrintStmt;

class StmtVisitor
{
public:
    virtual void visitExpressionStmt(ExpressionStmt&) = 0;
    virtual void visitPrintStmt(PrintStmt&) = 0;
};

class Stmt
{
public:
    virtual void accept(StmtVisitor&) = 0;
};

class ExpressionStmt : public Stmt
{
public:
    ExpressionStmt(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    void accept(StmtVisitor& v) override
    {
        return v.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

class PrintStmt : public Stmt
{
public:
    PrintStmt(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    void accept(StmtVisitor& v) override
    {
        return v.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

#endif // STMT_HPP_INCLUDED
