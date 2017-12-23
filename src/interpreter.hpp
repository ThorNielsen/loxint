#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

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
            statement->accept(*this);
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
};

#endif // INTERPRETER_HPP_INCLUDED

