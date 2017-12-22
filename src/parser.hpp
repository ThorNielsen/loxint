#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "expr.hpp"
#include "error.hpp"
#include <vector>


class Parser
{
public:
    Parser();
    ~Parser();

    std::unique_ptr<Expr> parse(std::vector<Token> tokens)
    {
        m_tokens = tokens;
        m_ctok = 0;
        return expression();
    }

private:
    using PExpr = std::unique_ptr<Expr>;
    PExpr expression()
    {
        auto expr = equality();
        if (!atEnd())
        {
            throw LoxError("Multiple expressions.");
        }
        return expr;
    }
    PExpr equality()
    {
        PExpr left = comparison();
        while (match({TokenType::BangEqual, TokenType::EqualEqual}))
        {
            Token oper = previous();
            PExpr right = comparison();
            left = PExpr(new Binary(std::move(left), oper, std::move(right)));
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
            left = PExpr(new Binary(std::move(left), oper, std::move(right)));
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
            left = PExpr(new Binary(std::move(left), oper, std::move(right)));
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
            left = PExpr(new Binary(std::move(left), oper, std::move(right)));
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
            return PExpr(new Unary(oper, std::move(right)));
        }
        return primary();
    }
    PExpr primary()
    {
        if (match({TokenType::False, TokenType::True,
                   TokenType::Nil, TokenType::Number,
                   TokenType::String}))
        {
            return PExpr(new Literal(previous()));
        }
        if (match({TokenType::LeftParen}))
        {
            PExpr expr = expression();
            consume(TokenType::RightParen, "No matching end paren.");
            return PExpr(new Grouping(std::move(expr)));
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

