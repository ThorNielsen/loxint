// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef EXPR_HPP_INCLUDED
#define EXPR_HPP_INCLUDED
#include "token.hpp"
#include <memory>

using ExprRetType = std::string;

class Binary;
class Grouping;
class Literal;
class Unary;

class ExprVisitor
{
public:
    virtual std::string visitBinaryExpr(Binary&) = 0;
    virtual std::string visitGroupingExpr(Grouping&) = 0;
    virtual std::string visitLiteralExpr(Literal&) = 0;
    virtual std::string visitUnaryExpr(Unary&) = 0;
};

class Expr
{
public:
    virtual std::string accept(ExprVisitor&) = 0;
};

class Binary : public Expr
{
public:
    Binary(std::unique_ptr<Expr>&& left_, Token oper_, std::unique_ptr<Expr>&& right_)
    {
        left = std::move(left_);
        oper = oper_;
        right = std::move(right_);
    }

    std::string accept(ExprVisitor& v) override
    {
        return v.visitBinaryExpr(*this);
    }

    std::unique_ptr<Expr> left;
    Token oper;
    std::unique_ptr<Expr> right;
};

class Grouping : public Expr
{
public:
    Grouping(std::unique_ptr<Expr>&& expr_)
    {
        expr = std::move(expr_);
    }

    std::string accept(ExprVisitor& v) override
    {
        return v.visitGroupingExpr(*this);
    }

    std::unique_ptr<Expr> expr;
};

class Literal : public Expr
{
public:
    Literal(Token value_)
    {
        value = value_;
    }

    std::string accept(ExprVisitor& v) override
    {
        return v.visitLiteralExpr(*this);
    }

    Token value;
};

class Unary : public Expr
{
public:
    Unary(Token oper_, std::unique_ptr<Expr>&& right_)
    {
        oper = oper_;
        right = std::move(right_);
    }

    std::string accept(ExprVisitor& v) override
    {
        return v.visitUnaryExpr(*this);
    }

    Token oper;
    std::unique_ptr<Expr> right;
};

#endif // EXPR_HPP_INCLUDED
