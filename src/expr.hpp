// Warning: This code is auto-generated and may be changed at any
// time by a script. Edits will be reverted.
#ifndef EXPR_HPP_INCLUDED
#define EXPR_HPP_INCLUDED
#include "token.hpp"
#include "loxobject.hpp"
#include <memory>

using ExprRetType = LoxObject;

class AssignmentExpr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;
class VariableExpr;

class ExprVisitor
{
public:
    virtual ExprRetType visitAssignmentExpr(AssignmentExpr&) = 0;
    virtual ExprRetType visitBinaryExpr(BinaryExpr&) = 0;
    virtual ExprRetType visitGroupingExpr(GroupingExpr&) = 0;
    virtual ExprRetType visitLiteralExpr(LiteralExpr&) = 0;
    virtual ExprRetType visitUnaryExpr(UnaryExpr&) = 0;
    virtual ExprRetType visitVariableExpr(VariableExpr&) = 0;
};

class Expr
{
public:
    virtual LoxObject accept(ExprVisitor&) = 0;
};

class AssignmentExpr : public Expr
{
public:
    AssignmentExpr(Token name_, std::unique_ptr<Expr>&& val_)
    {
        name = name_;
        val = std::move(val_);
    }

    ExprRetType accept(ExprVisitor& v) override
    {
        return v.visitAssignmentExpr(*this);
    }

    Token name;
    std::unique_ptr<Expr> val;
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

    ExprRetType accept(ExprVisitor& v) override
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

    ExprRetType accept(ExprVisitor& v) override
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

    ExprRetType accept(ExprVisitor& v) override
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

    ExprRetType accept(ExprVisitor& v) override
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

    ExprRetType accept(ExprVisitor& v) override
    {
        return v.visitVariableExpr(*this);
    }

    Token name;
};

#endif // EXPR_HPP_INCLUDED
