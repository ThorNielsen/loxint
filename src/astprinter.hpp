#ifndef ASTPRINTER_HPP_INCLUDED
#define ASTPRINTER_HPP_INCLUDED

#include "expr.hpp"

class ASTPrinter : public ExprVisitor
{
public:
    ASTPrinter() {}
    ~ASTPrinter() {}

    ExprRetType visitBinaryExpr(Binary& bin) override
    {
        return parenthesize(bin.oper.lexeme, {bin.left.get(), bin.right.get()});
    }
    ExprRetType visitGroupingExpr(Grouping& grp) override
    {
        return parenthesize("group", {grp.expr.get()});
    }
    ExprRetType visitLiteralExpr(Literal& lit)
    {
        return LoxObject((std::string)lit.value);
    }
    ExprRetType visitUnaryExpr(Unary& un)
    {
        return parenthesize(un.oper.lexeme, {un.right.get()});
    }

    void print(Expr* expr)
    {
        std::cout << expr->accept(*this).string << std::endl;
    }
private:
    std::string parenthesize(std::string name, std::vector<Expr*> exprs)
    {
        std::string s;
        s += "(" + name;
        for (Expr* expr : exprs)
        {
            s += " ";
            s += expr->accept(*this).string;
        }
        s += ")";
        return s;
    }
};

#endif // ASTPRINTER_HPP_INCLUDED

