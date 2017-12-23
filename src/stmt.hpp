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
class VarStmt;

class StmtVisitor
{
public:
    virtual void visitExpressionStmt(ExpressionStmt&) = 0;
    virtual void visitPrintStmt(PrintStmt&) = 0;
    virtual void visitVarStmt(VarStmt&) = 0;
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
        v.visitExpressionStmt(*this);
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
        v.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

class VarStmt : public Stmt
{
public:
    VarStmt(Token name_, std::unique_ptr<Expr>&& init_)
    {
        name = name_;
        init = std::move(init_);
    }

    void accept(StmtVisitor& v) override
    {
        v.visitVarStmt(*this);
    }

    Token name;
    std::unique_ptr<Expr> init;
};

#endif // STMT_HPP_INCLUDED
