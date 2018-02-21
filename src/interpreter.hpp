#ifndef INTERPRETER_HPP_INCLUDED
#define INTERPRETER_HPP_INCLUDED

#include "callable.hpp"
#include "environment.hpp"
#include "expr.hpp"
#include "loxobject.hpp"
#include "stmt.hpp"
#include <map>
#include <memory>
#include <set>
#include <vector>

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(args)...));
}

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
    Interpreter()
    {
        m_destroying = false;
        m_globs = Environment::createNew(nullptr);
        m_env = m_globs;
        std::unique_ptr<Callable> clock{static_cast<Callable*>(new TimeFunction())};
        auto* loc = clock.get();
        m_callables[loc] = {std::move(clock), 0};
        m_globs->assign(loc->name(), LoxObject(loc, this));
    }
    ~Interpreter()
    {
        // Manually delete objects since functions assume their interpreter is
        // valid when they are destroyed.
        m_globs->clear();
        m_env->clear();
        // Note: This is a hack to avoid double-destruction.
        m_destroying = true;
        m_instances.clear();
        m_classes.clear();
        m_callables.clear();
        m_funcenvs.clear();
    }

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
        LoxObject super;
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

        auto classy = make_unique<LoxClass>(&cs, this, super.classy, m_env);
        auto* loc = classy.get();
        m_classes[loc] = {std::move(classy), 0};
        if (cs.super != nullptr)
        {
            m_env = m_env->enclosing;
        }
        m_env->assign(cs.name.lexeme, LoxObject(loc, this));
    }

    StmtRetType visitExpressionStmt(ExpressionStmt& es) override
    {
        es.expr->accept(*this);
    }

    StmtRetType visitFunctionStmt(FunctionStmt& fs) override
    {
        auto* func = createFunction(&fs, m_env);
        m_env->assign(fs.name.lexeme, LoxObject(func, this));
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
        else
        {
            if (!m_globs->exists(as.name.lexeme))
            {
                throw std::runtime_error("Cannot assign to undeclared variable.");
            }
            m_globs->assign(as.name.lexeme, value);
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
        auto object = m_env->get(m_locals[&se]-1, "this");
        auto super = m_env->get(m_locals[&se], "super");
        return super.classy->function(se.method, object.instance);
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

    void registerFunction(LoxFunction* func, PEnvironment env)
    {
        if (m_funcenvs.find(func) != m_funcenvs.end())
        {
            throw std::runtime_error("Function already registered.");
        }
        m_funcenvs[func] = env;
    }

    void deleteFunction(LoxFunction* func)
    {
        if (m_funcenvs.find(func) == m_funcenvs.end())
        {
            std::cerr << "\033[31m" << func << "\033[0m\n";
            throw std::runtime_error("Failed to find function to delete.\n");
        }
        m_funcenvs.erase(func);
    }

    PEnvironment getFunctionEnvironment(LoxFunction* func)
    {
        return m_funcenvs[func];
    }

    void addUser(Callable* c)
    {
        ++m_callables[c].second;
    }

    void addUser(LoxClass* lc)
    {
        ++m_classes[lc].second;
    }

    void addUser(LoxInstance* li)
    {
        ++m_instances[li].second;
    }

    void removeUser(Callable* c)
    {
        if (m_destroying) return;
        --m_callables[c].second;
        if (!m_callables[c].second)
        {
            m_callables.erase(c);
        }
    }

    void removeUser(LoxClass* lc)
    {
        if (m_destroying) return;
        --m_classes[lc].second;
        if (!m_classes[lc].second)
        {
            m_classes.erase(lc);
        }
    }

    void removeUser(LoxInstance* li)
    {
        if (m_destroying) return;
        --m_instances[li].second;
        if (!m_instances[li].second)
        {
            m_instances.erase(li);
        }
    }

    LoxFunction* createFunction(FunctionStmt* fs, PEnvironment env,
                                bool classInit = false)
    {
        auto func = make_unique<LoxFunction>(fs, this, env, classInit);
        auto* loc = func.get();
        m_callables[loc] = {std::move(func), 0};
        return loc;
    }

    LoxFunction* createFunction(LoxFunction* fun, PEnvironment env)
    {
        auto func = make_unique<LoxFunction>(*fun, env);
        auto* loc = func.get();
        m_callables[loc] = {std::move(func), 0};
        return loc;
    }

    LoxInstance* createInstance(LoxClass* lc)
    {
        auto instance = make_unique<LoxInstance>(*lc);
        auto* loc = instance.get();
        m_instances[loc] = {std::move(instance), 0};
        return loc;
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

    std::map<Callable*, std::pair<std::unique_ptr<Callable>, size_t>> m_callables;
    std::map<LoxClass*, std::pair<std::unique_ptr<LoxClass>, size_t>> m_classes;
    std::map<LoxInstance*, std::pair<std::unique_ptr<LoxInstance>, size_t>> m_instances;

    std::map<LoxFunction*, PEnvironment> m_funcenvs;

    bool m_destroying;
};

#endif // INTERPRETER_HPP_INCLUDED

