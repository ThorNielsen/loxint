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
    case TokenType::EqualEqual: return LoxObject(l == r);
    case TokenType::BangEqual: return LoxObject(l != r);
    case TokenType::Less: return LoxObject(l < r);
    case TokenType::LessEqual: return LoxObject(l <= r);
    case TokenType::Greater: return LoxObject(l > r);
    case TokenType::GreaterEqual: return LoxObject(l >= r);
    default:
        throw LoxError("Unknown binary expression.");
    }
}

ExprRetType Interpreter::visitCallExpr(CallExpr& ce)
{
    m_calldepth += 1;
    if (m_calldepth >= MAX_CALL_DEPTH) {
        throw LoxError(error(ce.paren.line, "Maximum call depth reached."));
    }
    auto callee = ce.callee->accept(*this);
    std::vector<LoxObject> args;
    for (auto& arg : ce.args)
    {
        args.push_back(arg->accept(*this));
    }
    auto retCnt = returns();
    auto ret = callee(*this, args);
    if (retCnt != returns())
    {
        ret = m_returns.top();
        m_returns.pop();
    }

    return ret;
}

ExprRetType Interpreter::visitLogicalExpr(LogicalExpr& le)
{
    auto l = le.left->accept(*this);
    switch (le.oper.type)
    {
    case TokenType::And:
        if (!l) return LoxObject(false);
        return LoxObject((bool)le.right->accept(*this));
    case TokenType::Or:
        if (l) return LoxObject(true);
        return LoxObject((bool)le.right->accept(*this));
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
