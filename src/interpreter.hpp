#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "expr.hpp"
#include "loxobject.hpp"
#include "stmt.hpp"
#include <map>
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

    StmtRetType visitExpressionStmt(ExpressionStmt& es) override
    {
        es.expr->accept(*this);
    }
    StmtRetType visitPrintStmt(PrintStmt& ps) override
    {
        std::cout << (std::string)ps.expr->accept(*this) << "\n";
    }
    StmtRetType visitVarStmt(VarStmt& vs) override
    {
        m_vars[vs.name.lexeme] = vs.init->accept(*this);
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
        auto var = m_vars.find(ve.name.lexeme);
        if (var == m_vars.end())
        {
            throw LoxError("'" + ve.name.lexeme + "' was not declared.");
        }
        return var->second;
    }
private:
    std::map<std::string, LoxObject> m_vars;
};

#endif // INTERPRETER_HPP_INCLUDED

