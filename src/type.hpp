#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include "expr.hpp"
#include "stmt.hpp"
#include <iostream>

enum class Type
{
    AssignExpr, BinExpr, CallExpr, GetExpr, GroupExpr, LitExpr, LogicExpr,
    SetExpr, SuperExpr, ThisExpr, UnaryExpr, VarExpr,
    BlockStmt, ClassStmt, ExprStmt, FunctionStmt, IfStmt, PrintStmt, ReturnStmt,
    VarStmt, WhileStmt,
};

inline std::ostream& operator<<(std::ostream& ost, Type t)
{
    switch (t)
    {
    case Type::AssignExpr:   return ost << "AssignExpr";
    case Type::BinExpr:      return ost << "BinExpr";
    case Type::CallExpr:     return ost << "CallExpr";
    case Type::GetExpr:      return ost << "GetExpr";
    case Type::GroupExpr:    return ost << "GroupExpr";
    case Type::LitExpr:      return ost << "LitExpr";
    case Type::LogicExpr:    return ost << "LogicExpr";
    case Type::SetExpr:      return ost << "SetExpr";
    case Type::SuperExpr:    return ost << "SuperExpr";
    case Type::ThisExpr:     return ost << "ThisExpr";
    case Type::UnaryExpr:    return ost << "UnaryExpr";
    case Type::VarExpr:      return ost << "VarExpr";
    case Type::BlockStmt:    return ost << "BlockStmt";
    case Type::ClassStmt:    return ost << "ClassStmt";
    case Type::ExprStmt:     return ost << "ExprStmt";
    case Type::FunctionStmt: return ost << "FunctionStmt";
    case Type::IfStmt:       return ost << "IfStmt";
    case Type::PrintStmt:    return ost << "PrintStmt";
    case Type::ReturnStmt:   return ost << "ReturnStmt";
    case Type::VarStmt:      return ost << "VarStmt";
    case Type::WhileStmt:    return ost << "WhileStmt";
    }
    return ost;
}

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

    ExprRetType visitGetExpr(GetExpr&) override
    { m_type = Type::GetExpr; return LoxObject(); }

    ExprRetType visitGroupingExpr(GroupingExpr&) override
    { m_type = Type::GroupExpr; return LoxObject(); }

    ExprRetType visitLiteralExpr(LiteralExpr&) override
    { m_type = Type::LitExpr; return LoxObject(); }

    ExprRetType visitLogicalExpr(LogicalExpr&) override
    { m_type = Type::LogicExpr; return LoxObject(); }

    ExprRetType visitSetExpr(SetExpr&) override
    { m_type = Type::SetExpr; return LoxObject(); }

    ExprRetType visitSuperExpr(SuperExpr&) override
    { m_type = Type::SuperExpr; return LoxObject(); }

    ExprRetType visitThisExpr(ThisExpr&) override
    { m_type = Type::ThisExpr; return LoxObject(); }

    ExprRetType visitUnaryExpr(UnaryExpr&) override
    { m_type = Type::UnaryExpr; return LoxObject(); }

    ExprRetType visitVariableExpr(VariableExpr&) override
    { m_type = Type::VarExpr; return LoxObject(); }

    StmtRetType visitBlockStmt(BlockStmt&) override
    { m_type = Type::BlockStmt; }

    StmtRetType visitClassStmt(ClassStmt&) override
    { m_type = Type::ClassStmt; }

    StmtRetType visitExpressionStmt(ExpressionStmt&) override
    { m_type = Type::ExprStmt; }

    StmtRetType visitFunctionStmt(FunctionStmt&) override
    { m_type = Type::FunctionStmt; }

    StmtRetType visitIfStmt(IfStmt&) override
    { m_type = Type::IfStmt; }

    StmtRetType visitPrintStmt(PrintStmt&) override
    { m_type = Type::PrintStmt; }

    StmtRetType visitReturnStmt(ReturnStmt&) override
    { m_type = Type::ReturnStmt; }

    StmtRetType visitVariableStmt(VariableStmt&) override
    { m_type = Type::VarStmt; }

    StmtRetType visitWhileStmt(WhileStmt&) override
    { m_type = Type::WhileStmt; }

private:
    Type m_type;
};



#endif // TYPE_HPP_INCLUDED

