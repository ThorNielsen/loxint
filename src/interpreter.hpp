#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "callable.hpp"
#include "environment.hpp"
#include "expr.hpp"
#include "loxobject.hpp"
#include "stmt.hpp"
#include <map>
#include <memory>
#include <vector>

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
    Interpreter()
    {
        m_globs = Environment::createNew(nullptr);
        m_env = m_globs;
        auto clock = LoxCallable(new TimeFunction());
        m_globs->assign(clock->name(), clock);
    }
    ~Interpreter() { }

    void interpret(std::vector<std::unique_ptr<Stmt>>&& statements)
    {
        try
        {
            for (auto& statement : statements)
            {
                if (statement)
                {
                    m_stmts.emplace_back(std::move(statement));
                    m_stmts.back()->accept(*this);
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
        ScopeEnvironment newScope(m_env, Environment::createNew(m_env));
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
        auto func = std::make_shared<LoxFunction>(&fs, m_env);
        m_env->assign(fs.name.lexeme, LoxObject(func));
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
        if (rs.value != nullptr) throw rs.value->accept(*this);
        throw LoxObject();
    }

    StmtRetType visitVariableStmt(VariableStmt& vs) override
    {
        m_env->assign(vs.name.lexeme, vs.init->accept(*this));
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
        LoxObject value = as.val->accept(*this);
        if (m_locals.find(&as) != m_locals.end())
        {
            m_env->assign(m_locals[&as], as.name.lexeme, value);
        }
        return value;
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
        return getVariable(ve.name, &ve);
    }

    PEnvironment& getEnv()
    {
        return m_env;
    }

    void resolve(Expr* expr, size_t depth)
    {
        m_locals[expr] = depth;
    }

private:
    LoxObject& getVariable(Token name, Expr* expr)
    {
        if (m_locals.find(expr) != m_locals.end())
        {
            return m_env->get(m_locals[expr], name.lexeme);
        }
        return m_globs->get(name.lexeme);
    }
    PEnvironment m_globs;
    PEnvironment m_env;
    std::vector<std::unique_ptr<Stmt>> m_stmts;
    std::map<Expr*, size_t> m_locals;
};

#endif // INTERPRETER_HPP_INCLUDED

