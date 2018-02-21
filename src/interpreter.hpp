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
        m_globs = Environment::createNew(nullptr);
        m_env = m_globs;
        std::unique_ptr<Callable> clock{static_cast<Callable*>(new TimeFunction())};
        auto* loc = clock.get();
        m_callables[loc] = std::move(clock);
        m_calluse[loc] = 1;
        m_globs->assign(clock->name(), loc);
    }
    ~Interpreter()
    {
        // Manually delete objects since functions assume their interpreter is
        // valid when they are destroyed.
        m_globs->clear();
        m_env->clear();
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
        auto classy = make_unique<LoxClass>(&cs, this, super.classy, m_env);
        auto* ptr = classy.get();
        m_classes[ptr] = {std::move(classy), 0};
        m_derived[ptr] = {};
        m_instances[ptr] = {};
        if (cs.super != nullptr)
        {
            m_env = m_env->enclosing;
        }
        m_env->assign(cs.name.lexeme, LoxObject(ptr));
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
        auto func = make_unique<LoxFunction>(&fs, this, m_env);
        auto* loc = func.get();
        m_callables[loc] = std::move(func);
        m_calluse[loc] = 1;
        m_env->assign(fs.name.lexeme, LoxObject(loc));
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

    void registerFunction(LoxFunction* func, PEnvironment env)
    {
        std::cerr << "Registering " << func << "\n";
        if (m_funcenvs.find(func) != m_funcenvs.end())
        {
            throw std::runtime_error("Function already registered.");
        }
        m_funcenvs[func] = env;
    }

    void deleteFunction(LoxFunction* func)
    {
        std::cerr << "Deleting " << func << "\n";
        if (m_funcenvs.find(func) == m_funcenvs.end())
        {
            throw std::runtime_error("Failed to find function to delete.\n");
        }
        m_funcenvs.erase(func);
    }

    PEnvironment getFunctionEnvironment(LoxFunction* func)
    {
        return m_funcenvs[func];
    }

    void registerClass(LoxClass* classy)
    {
        std::cout << "Constructing " << classy << std::endl;
        if (m_classes.find(classy) == m_classes.end())
        {
            throw std::runtime_error("Tried to register non-existing class.");
        }
        ++m_classes[classy].second;
    }

    void deleteClass(LoxClass* classy)
    {
        std::cout << "Deleting " << classy << std::endl;
        if (m_classes.find(classy) == m_classes.end())
        {
            throw std::runtime_error("Tried to delete non-existing class.");
        }
        --m_classes[classy].second;
        if (!m_classes[classy].second)
        {
            if (m_derived.find(classy) != m_derived.end()
                && !m_derived[classy].empty())
            {
                throw std::runtime_error("Deleted super before subclass(es).");
            }
            m_derived.erase(classy);
            if (m_instances.find(classy) != m_instances.end()
                && !m_instances[classy].empty())
            {
                throw std::runtime_error("Deleted class before instances.");
            }
            m_instances.erase(classy);
            m_classes.erase(classy);
        }
    }

    void addUser(Callable*)
    {

    }

    void addUser(LoxClass*)
    {

    }

    void addUser(LoxInstance*)
    {

    }

    void removeUser(Callable*)
    {

    }

    void removeUser(LoxClass*)
    {

    }

    void removeUser(LoxInstance*)
    {

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

    std::map<Callable*, std::unique_ptr<Callable>> m_callables;
    std::map<Callable*, size_t> m_calluse;




    std::map<LoxFunction*, PEnvironment> m_funcenvs;

    std::map<LoxClass*, std::pair<std::unique_ptr<LoxClass>, size_t>> m_classes;
    // m_derived[super] -> {derived1*, derived2*, ...}
    std::map<LoxClass*, std::set<LoxClass*>> m_derived;
    std::map<LoxClass*, std::set<LoxInstance*>> m_instances;
};

#endif // INTERPRETER_HPP_INCLUDED

