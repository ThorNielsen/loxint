#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "callable.hpp"
#include "environment.hpp"
#include "expr.hpp"
#include "loxobject.hpp"
#include "stmt.hpp"
#include <memory>
#include <vector>

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
    Interpreter()
    {
        m_env = new Environment;
        auto clock = LoxCallable(new TimeFunction());
        m_env->createVar(clock->name(), clock);
    }
    ~Interpreter()
    {
        delete m_env;
    }

    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements)
    {
        try
        {
            for (auto& statement : statements)
            {
                if (statement)
                {
                    statement->accept(*this);
                }
            }
        }
        catch (LoxError err)
        {
            std::cerr << "Interpreting error: " << err.what() << "\n";
        }
        catch (LoxObject lo)
        {
            std::cerr << "Interpreting error: Unexpected return.\n";
        }
    }

    StmtRetType visitBlockStmt(BlockStmt& bs) override
    {
        ScopeEnvironment newScope(m_env);
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

    StmtRetType visitFunctionStmt(FunctionStmt& fs) override
    {
        if (fs.statements == nullptr)
        {
            throw LoxError("Function declaration has already been interpreted.");
        }
        m_env->createVar(fs.name.lexeme,
                         LoxObject(std::make_shared<LoxFunction>(&fs)));
    }

    StmtRetType visitIfStmt(IfStmt& is) override
    {
        if (is.cond->accept(*this))
        {
            is.thenBranch->accept(*this);
        }
        else if (is.elseBranch != nullptr)
        {
            is.elseBranch->accept(*this);
        }
    }

    StmtRetType visitPrintStmt(PrintStmt& ps) override
    {
        std::cout << (std::string)ps.expr->accept(*this) << "\n";
    }

    StmtRetType visitReturnStmt(ReturnStmt& rs) override
    {
        throw rs.value->accept(*this);
    }

    StmtRetType visitVariableStmt(VariableStmt& vs) override
    {
        m_env->createVar(vs.name.lexeme, vs.init->accept(*this));
    }

    StmtRetType visitWhileStmt(WhileStmt& ws) override
    {
        while (ws.cond->accept(*this))
        {
            ws.statement->accept(*this);
        }
    }


    ExprRetType visitAssignmentExpr(AssignmentExpr& as) override
    {
        return m_env->getVar(as.name.lexeme) = as.val->accept(*this);
    }

    ExprRetType visitBinaryExpr(BinaryExpr&) override;

    ExprRetType visitCallExpr(CallExpr&) override;

    ExprRetType visitGroupingExpr(GroupingExpr& grp) override
    {
        return grp.expr->accept(*this);
    }

    ExprRetType visitLiteralExpr(LiteralExpr& lit) override
    {
        return lit.value;
    }

    ExprRetType visitLogicalExpr(LogicalExpr&) override;

    ExprRetType visitUnaryExpr(UnaryExpr&) override;

    ExprRetType visitVariableExpr(VariableExpr& ve) override
    {
        return m_env->getVar(ve.name.lexeme);
    }

    Environment& getEnv()
    {
        return *m_env;
    }

private:
    Environment* m_env;
};

#endif // INTERPRETER_HPP_INCLUDED

