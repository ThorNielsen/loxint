#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <iostream>

enum class TokenType
{
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Minus, Plus, Semicolon, Slash, Star,

    Bang, BangEqual,
    Equal, EqualEqual,
    Greater, GreaterEqual,
    Less, LessEqual,

    Identifier, String, Number,

    And, Class, Else, False, Fun, For, If, Nil, Or,
    Print, Return, Super, This, True, Var, While,

    Eof,
};

inline std::string str(TokenType t)
{
    switch (t)
    {
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Minus: return "Minus";
        case TokenType::Plus: return "Plus";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Slash: return "Slash";
        case TokenType::Star: return "Star";
        case TokenType::Bang: return "Bang";
        case TokenType::BangEqual: return "BangEqual";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Identifier: return "Identifier";
        case TokenType::String: return "String";
        case TokenType::Number: return "Number";
        case TokenType::And: return "And";
        case TokenType::Class: return "Class";
        case TokenType::Else: return "Else";
        case TokenType::False: return "False";
        case TokenType::Fun: return "Fun";
        case TokenType::For: return "For";
        case TokenType::If: return "If";
        case TokenType::Nil: return "Nil";
        case TokenType::Or: return "Or";
        case TokenType::Print: return "Print";
        case TokenType::Return: return "Return";
        case TokenType::Super: return "Super";
        case TokenType::This: return "This";
        case TokenType::True: return "True";
        case TokenType::Var: return "Var";
        case TokenType::While: return "While";
        case TokenType::Eof: return "Eof";
    }
    return "Invalid token!";
}

extern std::map<std::string, TokenType> keywords;

struct Token
{
    std::string lexeme;
    TokenType type;
    size_t line;
};

inline std::ostream& operator<<(std::ostream& ost, const Token& tok)
{
    ost << str(tok.type) << " [" << tok.lexeme << "]";
    return ost;
}

inline void error(size_t line, std::string msg)
{
    std::cerr << "Error (" << (line+1) << "): " << msg << "\n";
}

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

