// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef EXPR_HPP_INCLUDED
#define EXPR_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>

using ExprRetType = LoxObject;

class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;
class VariableExpr;

class ExprVisitor
{
public:
    virtual LoxObject visitBinaryExpr(BinaryExpr&) = 0;
    virtual LoxObject visitGroupingExpr(GroupingExpr&) = 0;
    virtual LoxObject visitLiteralExpr(LiteralExpr&) = 0;
    virtual LoxObject visitUnaryExpr(UnaryExpr&) = 0;
    virtual LoxObject visitVariableExpr(VariableExpr&) = 0;
};

class Expr
{
public:
    virtual LoxObject accept(ExprVisitor&) = 0;
};

class BinaryExpr : public Expr
{
public:
    BinaryExpr(std::unique_ptr<Expr>&& left_, Token oper_, std::unique_ptr<Expr>&& right_)
    {
        left = std::move(left_);
        oper = oper_;
        right = std::move(right_);
    }

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitBinaryExpr(*this);
    }

    std::unique_ptr<Expr> left;
    Token oper;
    std::unique_ptr<Expr> right;
};

class GroupingExpr : public Expr
{
public:
    GroupingExpr(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitGroupingExpr(*this);
    }

    std::unique_ptr<Expr> expr;
};

class LiteralExpr : public Expr
{
public:
    LiteralExpr(LoxObject value_)
    {
        value = value_;
    }

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitLiteralExpr(*this);
    }

    LoxObject value;
};

class UnaryExpr : public Expr
{
public:
    UnaryExpr(Token oper_, std::unique_ptr<Expr>&& right_)
    {
        oper = oper_;
        right = std::move(right_);
    }

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitUnaryExpr(*this);
    }

    Token oper;
    std::unique_ptr<Expr> right;
};

class VariableExpr : public Expr
{
public:
    VariableExpr(Token name_)
    {
        name = name_;
    }

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitVariableExpr(*this);
    }

    Token name;
};

#endif // EXPR_HPP_INCLUDED
