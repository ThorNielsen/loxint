#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include "expr.hpp"
#include "stmt.hpp"

enum class Type
{
    AssignExpr, BinExpr, CallExpr, GroupExpr, LitExpr, LogicExpr,
    UnaryExpr, VarExpr,
    BlockStmt, ExprStmt, FunctionStmt, IfStmt, PrintStmt, VarStmt, WhileStmt
};

class TypeIdentifier : public ExprVisitor, public StmtVisitor
{
public:
    Type identify(Expr* expr)
    {
        expr->accept(*this);
        return m_type;
    }

    Type identify(Stmt* stmt)
    {
        stmt->accept(*this);
        return m_type;
    }


    ExprRetType visitAssignmentExpr(AssignmentExpr&) override
    { m_type = Type::AssignExpr; return LoxObject(); }

    ExprRetType visitBinaryExpr(BinaryExpr&) override
    { m_type = Type::BinExpr; return LoxObject(); }

    ExprRetType visitCallExpr(CallExpr&) override
    { m_type = Type::CallExpr; return LoxObject(); }

    ExprRetType visitGroupingExpr(GroupingExpr&) override
    { m_type = Type::GroupExpr; return LoxObject(); }

    ExprRetType visitLiteralExpr(LiteralExpr&) override
    { m_type = Type::LitExpr; return LoxObject(); }

    ExprRetType visitLogicalExpr(LogicalExpr&) override
    { m_type = Type::LogicExpr; return LoxObject(); }

    ExprRetType visitUnaryExpr(UnaryExpr&) override
    { m_type = Type::UnaryExpr; return LoxObject(); }

    ExprRetType visitVariableExpr(VariableExpr&) override
    { m_type = Type::VarExpr; return LoxObject(); }

    StmtRetType visitBlockStmt(BlockStmt&) override
    { m_type = Type::BlockStmt; }

    StmtRetType visitExpressionStmt(ExpressionStmt&) override
    { m_type = Type::ExprStmt; }

    StmtRetType visitFunctionStmt(FunctionStmt&) override
    { m_type = Type::FunctionStmt; }

    StmtRetType visitIfStmt(IfStmt&) override
    { m_type = Type::IfStmt; }

    StmtRetType visitPrintStmt(PrintStmt&) override
    { m_type = Type::PrintStmt; }

    StmtRetType visitVariableStmt(VariableStmt&) override
    { m_type = Type::VarStmt; }

    StmtRetType visitWhileStmt(WhileStmt&) override
    { m_type = Type::WhileStmt; }

private:
    Type m_type;
};



#endif // TYPE_HPP_INCLUDED

