#ifndef RESOLVER_HPP_INCLUDED
#define RESOLVER_HPP_INCLUDED

#include "error.hpp"
#include "expr.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include <map>
#include <string>
#include <vector>

class Resolver : public ExprVisitor, public StmtVisitor
{
public:
    Resolver() {}
    ~Resolver() {}
    bool resolve(std::vector<std::unique_ptr<Stmt>>& stmts, Interpreter& in)
    {
        interpreter = &in;
        m_ftype = FunctionType::None;
        m_ctype = ClassType::None;
        try
        {
            for (auto& stmt : stmts)
            {
                stmt->accept(*this);
            }
        }
        catch (LoxError err)
        {
            std::cerr << err.what() << "\n";
            interpreter = nullptr;
            return false;
        }
        interpreter = nullptr;
        return true;
    }

    StmtRetType visitBlockStmt(BlockStmt& bs) override
    {
        pushScope();
        for (auto& stmt : bs.statements)
        {
            resolve(stmt);
        }
        popScope();
    }
    StmtRetType visitClassStmt(ClassStmt& cs) override
    {
        declare(cs.name);
        define(cs.name);

        ClassType encctype = m_ctype;
        m_ctype = ClassType::Class;

        if (cs.super != nullptr)
        {
            m_ctype = ClassType::Subclass;
            resolve(cs.super);
            pushScope();
            m_names.back()["super"] = true;
        }

        pushScope();
        m_names.back()["this"] = true;
        for (auto& method : cs.methods)
        {
            FunctionType declType = FunctionType::Method;
            if (method->name.lexeme == "init")
            {
                declType = FunctionType::Initialiser;
            }
            resolveFunction(*method, declType);
        }
        popScope();

        if (cs.super != nullptr) popScope();
        m_ctype = encctype;
    }
    StmtRetType visitExpressionStmt(ExpressionStmt& es) override
    {
        resolve(es.expr);
    }
    StmtRetType visitFunctionStmt(FunctionStmt& fs) override
    {
        declare(fs.name);
        define(fs.name);
        resolveFunction(fs, FunctionType::Function);
    }
    StmtRetType visitIfStmt(IfStmt& is) override
    {
        resolve(is.cond);
        resolve(is.thenBranch);
        if (is.elseBranch != nullptr) resolve(is.elseBranch);
    }
    StmtRetType visitPrintStmt(PrintStmt& ps) override
    {
        resolve(ps.expr);
    }
    StmtRetType visitReturnStmt(ReturnStmt& rs) override
    {
        if (m_ftype == FunctionType::None)
        {
            throwError(rs.keyword.line, "Cannot return from top-level code.");
        }
        if (rs.value != nullptr)
        {
            if (m_ftype == FunctionType::Initialiser)
            {
                throwError(rs.keyword.line,
                           "Cannot return a value from an initialiser.");
            }
            resolve(rs.value);
        }
    }
    StmtRetType visitWhileStmt(WhileStmt& ws) override
    {
        resolve(ws.cond);
        resolve(ws.statement);
    }
    StmtRetType visitVariableStmt(VariableStmt& vs) override
    {
        declare(vs.name);
        if (vs.init)
        {
            resolve(vs.init);
        }
        define(vs.name);
    }

    ExprRetType visitAssignmentExpr(AssignmentExpr& as) override
    {
        resolve(as.val);
        resolveLocal(as, as.name);
        return LoxObject();
    }

    ExprRetType visitBinaryExpr(BinaryExpr& bs) override
    {
        resolve(bs.left);
        resolve(bs.right);
        return LoxObject();
    }

    ExprRetType visitCallExpr(CallExpr& ce) override
    {
        resolve(ce.callee);

        for (auto& arg : ce.args)
        {
            resolve(arg);
        }
        return LoxObject();
    }

    ExprRetType visitGetExpr(GetExpr& ge) override
    {
        resolve(ge.object);
        return LoxObject();
    }

    ExprRetType visitGroupingExpr(GroupingExpr& gs) override
    {
        resolve(gs.expr);
        return LoxObject();
    }

    ExprRetType visitLiteralExpr(LiteralExpr&) override
    {
        return LoxObject();
    }

    ExprRetType visitLogicalExpr(LogicalExpr& ls) override
    {
        resolve(ls.left);
        resolve(ls.right);
        return LoxObject();
    }

    ExprRetType visitSetExpr(SetExpr& se) override
    {
        resolve(se.value);
        resolve(se.object);
        return LoxObject();
    }

    ExprRetType visitSuperExpr(SuperExpr& se) override
    {
        if (m_ctype == ClassType::None)
        {
            throwError(se.keyword.line,
                       "Cannot use 'super' outside a class.");
        }
        if (m_ctype != ClassType::Subclass)
        {
            throwError(se.keyword.line,
                       "Cannot use 'super' in a class without a superclass.");
        }
        resolveLocal(se, se.keyword);
        return LoxObject();
    }

    ExprRetType visitThisExpr(ThisExpr& te) override
    {
        if (m_ctype == ClassType::None)
        {
            throwError(te.keyword.line,
                       "Cannot use 'this' outside of a class.");
        }
        resolveLocal(te, te.keyword);
        return LoxObject();
    }

    ExprRetType visitVariableExpr(VariableExpr& ve) override
    {
        if (!m_names.empty()
            && m_names.front().find(ve.name.lexeme) != m_names.front().end()
            && !m_names.front()[ve.name.lexeme])
        {
            throwError(ve.name.line,
                       "Variable refers to itself in initialisation.");
        }
        resolveLocal(ve, ve.name);
        return LoxObject();
    }

    ExprRetType visitUnaryExpr(UnaryExpr& ue) override
    {
        resolve(ue.right);
        return LoxObject();
    }
private:
    enum class FunctionType
    {
        None, Function, Method, Initialiser,
    };
    enum class ClassType
    {
        None, Class, Subclass
    };
    void resolveLocal(Expr& expr, Token name)
    {
        size_t cscope = 0;
        for (auto scope = m_names.rbegin(); scope != m_names.rend(); ++scope)
        {
            if (scope->find(name.lexeme) != scope->end())
            {
                interpreter->resolve(&expr, cscope);
                return;
            }
            ++cscope;
        }
    }
    void resolveFunction(FunctionStmt& func, FunctionType ftype)
    {
        auto oldft = m_ftype;
        m_ftype = ftype;
        pushScope();
        for (auto param : func.params)
        {
            declare(param);
            define(param);
        }
        for (auto& stmt : func.statements->statements)
        {
            stmt->accept(*this);
        }
        popScope();
        m_ftype = oldft;
    }
    void resolve(std::unique_ptr<Stmt>& stmt)
    {
        stmt->accept(*this);
    }
    void resolve(std::unique_ptr<Expr>& expr)
    {
        expr->accept(*this);
    }

    void declare(Token name)
    {
        if (m_names.empty()) return;
        if (m_names.back().find(name.lexeme) != m_names.back().end())
        {
            throwError(name.line,
                       "Variable '" + name.lexeme + "' was already declared.");
        }
        m_names.back()[name.lexeme] = false;
    }
    void define(Token name)
    {
        if (m_names.empty()) return;
        if (m_names.back().find(name.lexeme) == m_names.back().end())
        {
            throwError(name.line, "Undeclared name '" + name.lexeme + "'");
        }
        m_names.back()[name.lexeme] = true;
    }
    void pushScope()
    {
        m_names.push_back({});
    }
    void popScope()
    {
        m_names.pop_back();
    }
    size_t currentScope() const { return m_names.size(); }
    void throwError(size_t line, std::string msg)
    {
        throw LoxError("Resolver error ("
                       + std::to_string(line) + "): "
                       + msg);
    }
    std::vector<std::map<std::string, bool>> m_names;
    Interpreter* interpreter;
    FunctionType m_ftype;
    ClassType m_ctype;
};

#endif // RESOLVER_HPP_INCLUDED

