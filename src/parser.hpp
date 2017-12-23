#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include <vector>


class Parser
{
public:
    Parser();
    ~Parser();

    std::vector<std::unique_ptr<Stmt>> parse(std::vector<Token> tokens)
    {
        m_tokens = tokens;
        m_ctok = 0;
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!atEnd())
        {
            statements.emplace_back(statement());
        }
        return statements;
    }

private:
    using PExpr = std::unique_ptr<Expr>;
    using PStmt = std::unique_ptr<Stmt>;

    PStmt statement()
    {
        if (match({TokenType::Print}))
        {
            return printStatement();
        }
        return expressionStatement();
    }

    PStmt printStatement()
    {
        PStmt stmt = PStmt(new PrintStmt(expression()));
        consume(TokenType::Semicolon, "Expected ';' after expression.");
        return stmt;
    }

    PStmt expressionStatement()
    {
        PStmt stmt = PStmt(new ExpressionStmt(expression()));
        consume(TokenType::Semicolon, "Expected ';' after expression.");
        return stmt;
    }

    PExpr expression()
    {
        return equality();
    }
    PExpr equality()
    {
        PExpr left = comparison();
        while (match({TokenType::BangEqual, TokenType::EqualEqual}))
        {
            Token oper = previous();
            PExpr right = comparison();
            left = PExpr(new BinaryExpr(std::move(left), oper, std::move(right)));
        }
        return left;
    }
    PExpr comparison()
    {
        PExpr left = addition();
        while (match({TokenType::Greater, TokenType::GreaterEqual,
                      TokenType::Less, TokenType::LessEqual}))
        {
            Token oper = previous();
            PExpr right = addition();
            left = PExpr(new BinaryExpr(std::move(left), oper, std::move(right)));
        }
        return left;
    }
    PExpr addition()
    {
        PExpr left = multiplication();
        while (match({TokenType::Minus, TokenType::Plus}))
        {
            Token oper = previous();
            PExpr right = multiplication();
            left = PExpr(new BinaryExpr(std::move(left), oper, std::move(right)));
        }
        return left;
    }
    PExpr multiplication()
    {
        PExpr left = unary();
        while (match({TokenType::Star, TokenType::Slash}))
        {
            Token oper = previous();
            PExpr right = unary();
            left = PExpr(new BinaryExpr(std::move(left), oper, std::move(right)));
        }
        return left;
    }
    PExpr unary()
    {
        PExpr curr = nullptr;
        if (match({TokenType::Bang, TokenType::Minus}))
        {
            Token oper = previous();
            PExpr right = unary();
            return PExpr(new UnaryExpr(oper, std::move(right)));
        }
        return primary();
    }
    PExpr primary()
    {
        if (match({TokenType::False, TokenType::True,
                   TokenType::Nil, TokenType::Number,
                   TokenType::String}))
        {
            return PExpr(new LiteralExpr(previous()));
        }
        if (match({TokenType::LeftParen}))
        {
            PExpr expr = expression();
            consume(TokenType::RightParen, "No matching end paren.");
            return PExpr(new GroupingExpr(std::move(expr)));
        }
        throw std::runtime_error("Couldn't find a primary expression.");
    }

    bool match(std::vector<TokenType> tok)
    {
        for (auto& t : tok)
        {
            if (isCurrentEqual(t))
            {
                advance();
                return true;
            }
        }
        return false;
    }

    bool isCurrentEqual(TokenType t)
    {
        if (atEnd()) return false;
        return peek().type == t;
    }

    Token advance()
    {
        if (!atEnd()) ++m_ctok;
        return previous();
    }
    bool atEnd()
    {
        return peek().type == TokenType::Eof;
    }
    Token peek()
    {
        return m_tokens[m_ctok];
    }
    Token previous()
    {
        return m_tokens[m_ctok-1];
    }

    Token consume(TokenType type, std::string msg)
    {
        if (isCurrentEqual(type)) return advance();
        error(peek().line, "Parsing error: " + msg);
        throw LoxError("Parsing error.");
    }

    std::vector<Token> m_tokens;
    size_t m_ctok;
};

#endif // PARSER_HPP_INCLUDED

