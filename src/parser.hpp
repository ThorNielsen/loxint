#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "type.hpp"
#include <vector>


class Parser
{
public:
    Parser();
    ~Parser();

    std::vector<std::unique_ptr<Stmt>> parse(std::vector<Token> tokens,
                                             bool fromRepl = false)
    {
        m_tokens = tokens;
        m_ctok = 0;
        m_error = false;
        m_continuable = true;
        m_repl = fromRepl;
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!atEnd() && !hadError())
        {
            statements.emplace_back(declaration());
        }
        return statements;
    }

    bool continuable() const
    {
        return m_continuable;
    }

    bool parsedToEnd() const
    {
        return m_tokens[m_ctok].type == TokenType::Eof;
    }
    bool hadError() const
    {
        return m_error;
    }

private:
    using PExpr = std::unique_ptr<Expr>;
    using PStmt = std::unique_ptr<Stmt>;

    PStmt declaration()
    {
        try
        {
            if (match({TokenType::Class})) return classDeclaration();
            if (match({TokenType::Fun})) return function("function");
            if (match({TokenType::Var})) return varDeclaration();
            return statement();
        }
        catch (LoxError err)
        {
            m_error = true;
            if (m_repl && continuable()) return nullptr;
            synchronise();
            std::string e = err.what();
            if (e == "") std::cerr << "Unknown parsing error.\n";
            else         std::cerr << "Parsing e" << e.substr(1) << "\n";
            return nullptr;
        }
    }

    PStmt classDeclaration()
    {
        Token name = consume(TokenType::Identifier, "Expected class name.");
        PExpr super = nullptr;
        if (match({TokenType::Less}))
        {
            consume(TokenType::Identifier, "Expected superclass name.");
            super = PExpr(new VariableExpr(previous()));
        }
        consume(TokenType::LeftBrace, "Expect '{' before class definition.");
        std::vector<std::unique_ptr<FunctionStmt>> methods;
        while (!atEnd() && !isCurrentEqual(TokenType::RightBrace))
        {
            auto m = function("method");
            methods.emplace_back(static_cast<FunctionStmt*>(m.release()));
        }
        consume(TokenType::RightBrace, "Expect '}' to end class definition.");
        return PStmt(new ClassStmt(name, std::move(super), std::move(methods)));
    }

    PStmt function(std::string kind)
    {
        Token name = consume(TokenType::Identifier,
                             "Expected " + kind + " name.");
        consume(TokenType::LeftParen, "Expected '(' after " + kind + " name.");
        decltype(FunctionStmt::params) params;
        if (!isCurrentEqual(TokenType::RightParen))
        {
            do
            {
                params.push_back(consume(TokenType::Identifier,
                                         "Expected parameter name."));
            } while (match({TokenType::Comma}));
        }
        consume(TokenType::RightParen, "Expected ')' after parameters.");
        consume(TokenType::LeftBrace, "Expected '{' to start " + kind + ".");
        std::unique_ptr<BlockStmt> body =
            std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>
                                           (block().release()));
        return PStmt(new FunctionStmt(name,
                                      params,
                                      std::move(body)));
    }

    PStmt varDeclaration()
    {
        Token name = consume(TokenType::Identifier, "Expected variable name.");

        PExpr init = PExpr(new LiteralExpr(LoxObject()));
        if (match({TokenType::Equal}))
        {
            init = expression();
        }
        consume(TokenType::Semicolon,
                "Expected ';' after variable declaration.");
        return PStmt(new VariableStmt(name, std::move(init)));
    }

    PStmt statement()
    {
        if (match({TokenType::LeftBrace}))
        {
            return block();
        }
        if (match({TokenType::Print}))
        {
            return printStatement();
        }
        if (match({TokenType::Return}))
        {
            return returnStatement();
        }
        if (match({TokenType::If}))
        {
            return ifStatement();
        }
        if (match({TokenType::While}))
        {
            return whileStatement();
        }
        if (match({TokenType::For}))
        {
            return forStatement();
        }
        return expressionStatement();
    }

    PStmt block()
    {
        std::vector<PStmt> statements;
        while (!isCurrentEqual(TokenType::RightBrace) && !atEnd())
        {
            statements.push_back(declaration());
        }
        consume(TokenType::RightBrace, "Expected '}' to end block.");
        return PStmt(new BlockStmt(std::move(statements)));
    }

    PStmt expressionStatement()
    {
        PStmt stmt = PStmt(new ExpressionStmt(expression()));
        consume(TokenType::Semicolon, "Expected ';' after expression.");
        return stmt;
    }

    PStmt returnStatement()
    {
        Token keyword = previous();
        PExpr val;
        if (!isCurrentEqual(TokenType::Semicolon))
        {
            val = expression();
        }
        consume(TokenType::Semicolon, "Expected ';' after return expression.");
        return PStmt(new ReturnStmt(keyword, std::move(val)));
    }

    PStmt forStatement()
    {
        consume(TokenType::LeftParen, "Expected '(' after for.");
        PStmt init;
        if (!match({TokenType::Semicolon}))
        {
            if (match({TokenType::Var}))
            {
                init = varDeclaration();
            }
            else
            {
                init = expressionStatement();
            }
        }

        PExpr cond = PExpr(new LiteralExpr(true));
        if (!match({TokenType::Semicolon}))
        {
            cond = expression();
            consume({TokenType::Semicolon},
                    "Expected semicolon after for-condition.");
        }

        PStmt update;
        if (!match({TokenType::RightParen}))
        {
            update = PStmt(new ExpressionStmt(expression()));
            consume(TokenType::RightParen,
                    "Expected ')' to end for-conditions.");
        }

        PStmt body = statement();
        if (update)
        {
            decltype(BlockStmt::statements) stmts;
            stmts.emplace_back(std::move(body));
            stmts.emplace_back(std::move(update));
            body = PStmt(new BlockStmt(std::move(stmts)));
        }
        body = PStmt(new WhileStmt(std::move(cond), std::move(body)));
        if (init)
        {
            decltype(BlockStmt::statements) stmts;
            stmts.emplace_back(std::move(init));
            stmts.emplace_back(std::move(body));
            return PStmt(new BlockStmt(std::move(stmts)));
        }
        return body;
    }

    PStmt ifStatement()
    {
        consume(TokenType::LeftParen, "Expected '(' after if.");
        PExpr cond = expression();
        consume(TokenType::RightParen, "Expected ')' to end if-condition.");
        PStmt thenBranch = statement();
        PStmt elseBranch;
        if (match({TokenType::Else}))
        {
            elseBranch = statement();
        }
        return PStmt(new IfStmt(std::move(cond),
                                std::move(thenBranch),
                                std::move(elseBranch)));
    }

    PStmt printStatement()
    {
        PStmt stmt = PStmt(new PrintStmt(expression()));
        consume(TokenType::Semicolon, "Expected ';' after expression.");
        return stmt;
    }

    PStmt whileStatement()
    {
        consume(TokenType::LeftParen, "Expected '(' after while.");
        PExpr cond = expression();
        consume(TokenType::RightParen, "Expected ')' to end while-condition.");
        PStmt stmt = statement();
        return PStmt(new WhileStmt(std::move(cond), std::move(stmt)));
    }


    PExpr expression()
    {
        return assignment();
    }
    PExpr assignment()
    {
        PExpr left = lor();
        if (match({TokenType::Equal}))
        {
            PExpr val = assignment();
            TypeIdentifier ti;
            if (ti.identify(left.get()) == Type::VarExpr)
            {
                Token name = static_cast<VariableExpr*>(left.get())->name;
                return PExpr(new AssignmentExpr(name, std::move(val)));
            }
            if (ti.identify(left.get()) == Type::GetExpr)
            {
                auto get = std::unique_ptr<GetExpr>(static_cast<GetExpr*>(left.release()));
                return PExpr(new SetExpr(std::move(get->object), get->name, std::move(val)));
            }
            throw LoxError(error(previous().line,
                                 "Invalid assignment target."));
        }
        return left;
    }
    PExpr lor()
    {
        PExpr left = land();
        while (match({TokenType::Or}))
        {
            Token oper = previous();
            PExpr right = land();
            left = PExpr(new LogicalExpr(std::move(left), oper, std::move(right)));
        }
        return left;
    }
    PExpr land()
    {
        PExpr left = equality();
        while (match({TokenType::And}))
        {
            Token oper = previous();
            PExpr right = equality();
            left = PExpr(new LogicalExpr(std::move(left), oper, std::move(right)));
        }
        return left;
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
        return call();
    }
    PExpr call()
    {
        PExpr expr = primary();
        while (true)
        {
            if (match({TokenType::LeftParen}))
            {
                expr = finishCall(std::move(expr));
            }
            else if (match({TokenType::Dot}))
            {
                Token name = consume(TokenType::Identifier,
                                     "Expected property name after '.'.");
                expr = PExpr(new GetExpr(std::move(expr), name));
            }
            else
            {
                break;
            }
        }
        return expr;
    }
    PExpr primary()
    {
        if (match({TokenType::False, TokenType::True,
                   TokenType::Nil, TokenType::Number,
                   TokenType::String}))
        {
            return PExpr(new LiteralExpr(previous()));
        }
        if (match({TokenType::This}))
        {
            return PExpr(new ThisExpr(previous()));
        }
        if (match({TokenType::Identifier}))
        {
            return PExpr(new VariableExpr(previous()));
        }
        if (match({TokenType::LeftParen}))
        {
            PExpr expr = expression();
            consume(TokenType::RightParen, "Expected ')' after expression.");
            return PExpr(new GroupingExpr(std::move(expr)));
        }
        if (!atEnd()) m_continuable = false;
        throw LoxError(error(peek().line,
                             "Couldn't find a primary expression."));
    }

    PExpr finishCall(PExpr func)
    {
        decltype(CallExpr::args) args;
        if (!isCurrentEqual(TokenType::RightParen))
        {
            do
            {
                args.emplace_back(expression());
            } while (match({TokenType::Comma}));
        }
        Token paren = consume(TokenType::RightParen,
                              "Expected ')' after function call.");
        return PExpr(new CallExpr(std::move(func), paren, std::move(args)));
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
        if (!atEnd()) m_continuable = false;
        throw LoxError(error(peek().line, msg));
    }

    void synchronise();

    std::vector<Token> m_tokens;
    size_t m_ctok;
    bool m_error;
    bool m_continuable;
    bool m_repl;
};

#endif // PARSER_HPP_INCLUDED

