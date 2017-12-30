#include "parser.hpp"

Parser::Parser()
    : m_tokens{{"", TokenType::Eof, 0}}, m_ctok{}, m_error{false},
      m_continuable{true}, m_repl{false}
{
}

Parser::~Parser()
{
}

#pragma GCC diagnostic ignored "-Wswitch-enum"

void Parser::synchronise()
{
    advance();
    while (!atEnd())
    {
        if (previous().type == TokenType::Semicolon) return;
        switch (peek().type)
        {
        case TokenType::Class:
        case TokenType::Fun:
        case TokenType::For:
        case TokenType::If:
        case TokenType::Else:
        case TokenType::While:
        case TokenType::Print:
        case TokenType::Return:
        case TokenType::Var:
            return;
        default:
            advance();
        }
    }
}
