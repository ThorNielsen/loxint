#ifndef ASTPRINTER_HPP_INCLUDED
#define ASTPRINTER_HPP_INCLUDED

#include "expr.hpp"
#include "stmt.hpp"

class ASTPrinter : public ExprVisitor, public StmtVisitor
{
public:
    ASTPrinter() {}
    ~ASTPrinter() {}

    void visitBlockStmt(BlockStmt& bs) override
    {
        m_ast += "(block ";
        for (auto& stmt : bs.statements)
        {
            stmt->accept(*this);
        }
        m_ast += ")";
    }
    void visitClassStmt(ClassStmt& cs) override
    {
        m_ast += "(class " + cs.name.lexeme;
        if (cs.super)
        {
            m_ast += " < ";
            cs.super->accept(*this);
        }
        for (auto& func : cs.methods)
        {
            m_ast += " ";
            func->accept(*this);
        }
        m_ast += ")";
    }
    void visitExpressionStmt(ExpressionStmt& es) override
    {
        parenthesize(";", {es.expr.get()});
    }

    void visitFunctionStmt(FunctionStmt& fs) override
    {
        m_ast += "(fun " + fs.name.lexeme + "(";
        for (size_t i = 0; i < fs.params.size(); ++i)
        {
            if (i) m_ast += " ";
            m_ast += fs.params[i].lexeme;
        }
        m_ast += "){";
        for (auto& stmt : fs.statements->statements)
        {
            stmt->accept(*this);
        }
        m_ast += "})";
    }
    void visitIfStmt(IfStmt& is) override
    {
        if (is.elseBranch == nullptr) m_ast += "(if ";
        else m_ast += "(if-else ";
        is.cond->accept(*this);
        m_ast += " ";
        is.thenBranch->accept(*this);
        if (is.elseBranch != nullptr)
        {
            m_ast += " ";
            is.elseBranch->accept(*this);
        }
        m_ast += ")";
    }

    void visitPrintStmt(PrintStmt& ps) override
    {
        parenthesize("print", {ps.expr.get()});
    }

    void visitReturnStmt(ReturnStmt& rs) override
    {
        if (rs.value != nullptr)
        {
            parenthesize("return", {rs.value.get()});
        }
    }

    void visitVariableStmt(VariableStmt& vs) override
    {
        m_ast += "(var " + vs.name.lexeme;
        if (vs.init != nullptr)
        {
            m_ast += " ";
            vs.init->accept(*this);
        }
        m_ast += ")";
    }

    void visitWhileStmt(WhileStmt& ws) override
    {
        m_ast += "(while ";
        ws.cond->accept(*this);
        m_ast += " ";
        ws.statement->accept(*this);
        m_ast += ")";
    }

    ExprRetType visitAssignmentExpr(AssignmentExpr& as) override
    {
        parenthesize("=" + as.name.lexeme, {as.val.get()});
        return LoxObject();
    }
    ExprRetType visitBinaryExpr(BinaryExpr& bin) override
    {
        parenthesize(bin.oper.lexeme, {bin.left.get(), bin.right.get()});
        return LoxObject();
    }
    ExprRetType visitCallExpr(CallExpr& ce) override
    {
        m_ast += "(call ";
        ce.callee->accept(*this);
        for (auto& expr : ce.args)
        {
            m_ast += " ";
            expr->accept(*this);
        }
        m_ast += ")";
        return LoxObject();
    }
    ExprRetType visitGetExpr(GetExpr& ge) override
    {
        m_ast += "(. ";
        ge.object->accept(*this);
        m_ast += " " + ge.name.lexeme + ")";
        return LoxObject();
    }
    ExprRetType visitGroupingExpr(GroupingExpr& grp) override
    {
        parenthesize("group", {grp.expr.get()});
        return LoxObject();
    }
    ExprRetType visitLiteralExpr(LiteralExpr& lit) override
    {
        m_ast += (std::string)lit.value;
        return LoxObject();
    }
    ExprRetType visitLogicalExpr(LogicalExpr& le) override
    {
        parenthesize(le.oper.lexeme, {le.left.get(), le.right.get()});
        return LoxObject();
    }
    ExprRetType visitSetExpr(SetExpr& se) override
    {
        m_ast += "(= ";
        se.object->accept(*this);
        m_ast += " " + se.name.lexeme + " ";
        se.value->accept(*this);
        m_ast += ")";
        return LoxObject();
    }
    ExprRetType visitSuperExpr(SuperExpr& se) override
    {
        m_ast += "(super " + se.method.lexeme + ")";
        return LoxObject();
    }
    ExprRetType visitThisExpr(ThisExpr&) override
    {
        m_ast += "this";
        return LoxObject();
    }
    ExprRetType visitUnaryExpr(UnaryExpr& un) override
    {
        parenthesize(un.oper.lexeme, {un.right.get()});
        return LoxObject();
    }
    ExprRetType visitVariableExpr(VariableExpr& ve) override
    {
        m_ast += ve.name.lexeme;
        return LoxObject();
    }

    void print(Expr* expr)
    {
        expr->accept(*this);
        std::cout << m_ast << std::endl;
        m_ast = "";
    }

    void print(Stmt* stmt)
    {
        stmt->accept(*this);
        std::cout << m_ast << std::endl;
        m_ast = "";
    }
private:
    void parenthesize(std::string name, std::vector<Expr*> exprs)
    {
        m_ast += "(" + name;
        for (auto expr : exprs)
        {
            m_ast += " ";
            expr->accept(*this);
        }
        m_ast += ")";
    }
    std::string m_ast;
};

#endif // ASTPRINTER_HPP_INCLUDED

