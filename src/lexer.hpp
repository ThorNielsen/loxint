#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "token.hpp"
#include "error.hpp"
#include <vector>

class Lexer
{
public:
    Lexer(std::string source)
        : m_src{source}, m_pos{0}, m_start{0}, m_line{0} {}
    std::vector<Token> scanTokens()
    {
        while (!atEnd())
        {
            scanToken();
        }
        m_tokens.push_back({"", TokenType::Eof, m_line});
        return m_tokens;
    }
private:
    bool atEnd()
    {
        return m_pos >= m_src.size();
    }
    char advance()
    {
        return m_src[m_pos++];
    }
    char peek()
    {
        if (atEnd()) return '\0';
        return m_src[m_pos];
    }
    char peekNext()
    {
        if (m_pos + 1 >= m_src.size()) return '\0';
        return m_src[m_pos + 1];
    }
    bool match(char c)
    {
        if (atEnd()) return false;
        if (m_src[m_pos] == c)
        {
            ++m_pos;
            return true;
        }
        return false;
    }
    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }
    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }
    bool isAlnum(char c)
    {
        return isAlpha(c) || isDigit(c);
    }
    void addToken(TokenType tt, std::string lexeme, size_t line)
    {
        m_tokens.push_back({lexeme, tt, line});
    }
    void addToken(TokenType tt)
    {
        addToken(tt, m_src.substr(m_start, m_pos - m_start), m_line);
    }
    void scanString()
    {
        auto line = m_line;
        bool escaped = false;
        std::string scanned = "";
        while ((escaped || peek() != '\"') && !atEnd())
        {
            if (peek() == '\n') ++m_line;
            if (escaped)
            {
                auto c = peek();
                switch(c)
                {
                case '\"': case '\'': case '\\':
                    scanned.push_back(c);
                    break;
                case 'n': scanned.push_back('\n'); break;
                case 'r': scanned.push_back('\r'); break;
                case 't': scanned.push_back('\t'); break;
                default:
                    error(m_line, "Erroneous escape.");
                    return;
                }
                escaped = false;
            }
            else
            {
                if (peek() == '\\') escaped = true;
                else
                {
                    scanned.push_back(peek());
                }
            }
            advance();
        }
        if (atEnd())
        {
            error(m_line, "Unterminated string.");
            return;
        }
        advance();

        addToken(TokenType::String, scanned, line);
    }
    void scanNumber()
    {
        while (isDigit(peek())) advance();
        if (peek() == '.' && isDigit(peekNext()))
        {
            advance();
            while (isDigit(peek())) advance();
        }
        addToken(TokenType::Number);
    }
    void scanIdentifier()
    {
        while (isAlnum(peek())) advance();
        std::string identifier = m_src.substr(m_start, m_pos - m_start);
        if (keywords.find(identifier) != keywords.end())
        {
            addToken(keywords[identifier], identifier, m_line);
        }
        else
        {
            addToken(TokenType::Identifier, identifier, m_line);
        }
    }
    void scanToken()
    {
        m_start = m_pos;
        char c = advance();
        switch (c)
        {
        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case '{': addToken(TokenType::LeftBrace); break;
        case '}': addToken(TokenType::RightBrace); break;
        case ',': addToken(TokenType::Comma); break;
        case '.': addToken(TokenType::Dot); break;
        case '-': addToken(TokenType::Minus); break;
        case '+': addToken(TokenType::Plus); break;
        case ';': addToken(TokenType::Semicolon); break;
        case '*': addToken(TokenType::Star); break;
        case '/':
            if (match('/'))
            {
                while (peek() != '\n' && !atEnd())
                {
                    advance();
                }
            }
            else if (match('*'))
            {
                while (!(peek() == '*' && peekNext() == '/') && !atEnd())
                {
                    if (peek() == '\n') ++m_line;
                    advance();
                }
                advance();
                advance();
            }
            else
            {
                addToken(TokenType::Slash);
            }
            break;

        case '!':
            addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
            break;
        case '=':
            addToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
            break;
        case '>':
            addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
        case '<':
            addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        break;

        case ' ': case '\t': case '\r':
            break;

        case '\n': ++m_line; break;
        case '\"': scanString(); break;

        default:
            if (isDigit(c))
            {
                scanNumber();
            }
            else if (isAlpha(c))
            {
                scanIdentifier();
            }
            else
            {
                error(m_line, "Unexpected character.");
            }
            break;
        }
    }
    std::vector<Token> m_tokens;
    std::string m_src;
    size_t m_pos;
    size_t m_start;
    size_t m_line;
};

#endif // LEXER_HPP_INCLUDED

