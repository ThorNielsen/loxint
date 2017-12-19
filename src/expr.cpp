// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#include "token.hpp"
#include <memory>

class Binary;
class Grouping;
class Literal;
class Unary;

class Visitor
{
public:
    void* visitBinaryExpr(Binary&);
    void* visitGroupingExpr(Grouping&);
    void* visitLiteralExpr(Literal&);
    void* visitUnaryExpr(Unary&);
};

class Expr
{
public:
    virtual void* accept(Visitor&) = 0;
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

    void* accept(Visitor& v) override
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

    void* accept(Visitor& v) override
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

    void* accept(Visitor& v) override
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

    void* accept(Visitor& v) override
    {
        return v.visitUnaryExpr(*this);
    }

    Token oper;
    std::unique_ptr<Expr> right;
};

