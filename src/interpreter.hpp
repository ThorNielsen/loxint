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
        auto clock = std::shared_ptr<Callable>(new TimeFunction());
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

    StmtRetType visitClassStmt(ClassStmt& cs) override
    {
        LoxObject super = nullptr;
        if (cs.super != nullptr)
        {
            super = cs.super->accept(*this);
            if (super.type != LoxType::Class)
            {
                throw LoxError("Superclass must be a class.");
            }
            m_env = Environment::createNew(m_env);
            m_env->assign("super", super);
        }
        auto classy = std::make_shared<LoxClass>(&cs, super.classy, m_env);
        if (cs.super != nullptr)
        {
            m_env = m_env->enclosing;
        }
        m_env->assign(cs.name.lexeme, LoxObject(classy));
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

    ExprRetType visitGetExpr(GetExpr& ge) override
    {
        return ge.object->accept(*this).get(ge.name);
    }

    ExprRetType visitGroupingExpr(GroupingExpr& grp) override
    {
        return grp.expr->accept(*this);
    }

    ExprRetType visitLiteralExpr(LiteralExpr& lit) override
    {
        return lit.value;
    }

    ExprRetType visitLogicalExpr(LogicalExpr&) override;

    ExprRetType visitSetExpr(SetExpr& se) override
    {
        return se.object->accept(*this).set(se.name, se.value->accept(*this));
    }

    ExprRetType visitSuperExpr(SuperExpr& se) override
    {
        auto super = m_env->get(m_locals[&se], "super").classy;
        auto object = m_env->get(m_locals[&se]-1, "this").instance;
        return super->function(se.method, object);
    }

    ExprRetType visitThisExpr(ThisExpr& te) override
    {
        return getVariable(te.keyword, &te);
    }

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

