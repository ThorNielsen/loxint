// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef STMT_HPP_INCLUDED
#define STMT_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>
#include <vector>

using StmtRetType = void;

class BlockStmt;
class ExpressionStmt;
class PrintStmt;
class VariableStmt;

class StmtVisitor
{
public:
    virtual StmtRetType visitBlockStmt(BlockStmt&) = 0;
    virtual StmtRetType visitExpressionStmt(ExpressionStmt&) = 0;
    virtual StmtRetType visitPrintStmt(PrintStmt&) = 0;
    virtual StmtRetType visitVariableStmt(VariableStmt&) = 0;
};

class Stmt
{
public:
    virtual void accept(StmtVisitor&) = 0;
};

class BlockStmt : public Stmt
{
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>>&& statements_)
    {
        statements = std::move(statements_);
    }

    StmtRetType accept(StmtVisitor& v) override
    {
        v.visitBlockStmt(*this);
    }

    std::vector<std::unique_ptr<Stmt>> statements;
};

class ExpressionStmt : public Stmt
{
public:
    ExpressionStmt(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    StmtRetType accept(StmtVisitor& v) override
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

    StmtRetType accept(StmtVisitor& v) override
    {
        v.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expr;
};

class VariableStmt : public Stmt
{
public:
    VariableStmt(Token name_, std::unique_ptr<Expr>&& init_)
    {
        name = name_;
        init = std::move(init_);
    }

    StmtRetType accept(StmtVisitor& v) override
    {
        v.visitVariableStmt(*this);
    }

    Token name;
    std::unique_ptr<Expr> init;
};

#endif // STMT_HPP_INCLUDED
