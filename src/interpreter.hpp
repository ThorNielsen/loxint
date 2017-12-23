#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "environment.hpp"
#include "expr.hpp"
#include "loxobject.hpp"
#include "stmt.hpp"
#include <memory>
#include <vector>

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
    Interpreter() {}
    ~Interpreter() {}

    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements)
    {
        for (auto& statement : statements)
        {
            if (statement)
            {
                statement->accept(*this);
            }
        }
    }

    StmtRetType visitBlockStmt(BlockStmt& bs) override
    {
        ScopeGuard newScope(m_env);
        for (auto& statement : bs.statements)
        {
            if (statement)
            {
                statement->accept(*this);
            }
        }
    }

    StmtRetType visitExpressionStmt(ExpressionStmt& es) override
    {
        es.expr->accept(*this);
    }
    StmtRetType visitPrintStmt(PrintStmt& ps) override
    {
        std::cout << (std::string)ps.expr->accept(*this) << "\n";
    }
    StmtRetType visitVariableStmt(VariableStmt& vs) override
    {
        m_env.createVar(vs.name.lexeme, vs.init->accept(*this));
    }
    ExprRetType visitAssignmentExpr(AssignmentExpr& as) override
    {
        return m_env.getVar(as.name.lexeme) = as.val->accept(*this);
    }
    ExprRetType visitBinaryExpr(BinaryExpr&) override;
    ExprRetType visitGroupingExpr(GroupingExpr& grp) override
    {
        return grp.expr->accept(*this);
    }
    ExprRetType visitLiteralExpr(LiteralExpr& lit) override
    {
        return lit.value;
    }
    ExprRetType visitUnaryExpr(UnaryExpr&) override;
    ExprRetType visitVariableExpr(VariableExpr& ve) override
    {
        return m_env.getVar(ve.name.lexeme);
    }
private:
    Environment m_env;
};

#endif // INTERPRETER_HPP_INCLUDED

