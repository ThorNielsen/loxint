#include "interpreter.hpp"
#include "error.hpp"

#pragma GCC diagnostic ignored "-Wswitch-enum"

ExprRetType Interpreter::visitBinaryExpr(BinaryExpr& bin)
{
    auto l = bin.left->accept(*this);
    auto r = bin.right->accept(*this);
    switch (bin.oper.type)
    {
    case TokenType::Plus: return l + r;
    case TokenType::Minus: return l - r;
    case TokenType::Star: return l * r;
    case TokenType::Slash: return l / r;
    case TokenType::EqualEqual: return l == r;
    case TokenType::BangEqual: return l != r;
    case TokenType::Less: return l < r;
    case TokenType::LessEqual: return l <= r;
    case TokenType::Greater: return l > r;
    case TokenType::GreaterEqual: return l >= r;
    default:
        throw LoxError("Unknown binary expression.");
    }
}

ExprRetType Interpreter::visitCallExpr(CallExpr& ce)
{
    auto callee = ce.callee->accept(*this);
    std::vector<LoxObject> args;
    for (auto& arg : ce.args)
    {
        args.push_back(arg->accept(*this));
    }
    return callee(*this, args);
}

ExprRetType Interpreter::visitLogicalExpr(LogicalExpr& le)
{
    auto l = le.left->accept(*this);
    switch (le.oper.type)
    {
    case TokenType::And:
        if (!l) return false;
        return (bool)le.right->accept(*this);
    case TokenType::Or:
        if (l) return true;
        return (bool)le.right->accept(*this);
    default:
        throw LoxError("Unknown logical expression.");
    }
}

ExprRetType Interpreter::visitUnaryExpr(UnaryExpr& un)
{
    auto eval = un.right->accept(*this);
    switch (un.oper.type)
    {
    case TokenType::Minus: return -eval;
    case TokenType::Bang: return !eval;
    default:
        throw LoxError("Invalid unary expression.");
    }
}
