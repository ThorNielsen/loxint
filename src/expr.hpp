// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef EXPR_HPP_INCLUDED
#define EXPR_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>

using ExprRetType = LoxObject;

class Binary;
class Grouping;
class Literal;
class Unary;

class ExprVisitor
{
public:
    virtual LoxObject visitBinaryExpr(Binary&) = 0;
    virtual LoxObject visitGroupingExpr(Grouping&) = 0;
    virtual LoxObject visitLiteralExpr(Literal&) = 0;
    virtual LoxObject visitUnaryExpr(Unary&) = 0;
};

class Expr
{
public:
    virtual LoxObject accept(ExprVisitor&) = 0;
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

    LoxObject accept(ExprVisitor& v) override
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

    LoxObject accept(ExprVisitor& v) override
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

    LoxObject accept(ExprVisitor& v) override
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

    LoxObject accept(ExprVisitor& v) override
    {
        return v.visitUnaryExpr(*this);
    }

    Token oper;
    std::unique_ptr<Expr> right;
};

#endif // EXPR_HPP_INCLUDED
