#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "lexer.hpp"

#include "astprinter.hpp"
#include "parser.hpp"

void run(std::string source)
{
    Lexer lex(source);
    auto tokens = lex.scanTokens();
    try
    {
        ASTPrinter asp;
        Parser p;
        asp.print(p.parse(tokens).get());
    }
    catch (std::runtime_error& err)
    {
        std::cerr << "Error: " << err.what() << "\n";
    }
}

void runPrompt()
{
    while (std::cin.good())
    {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line, '\n');
        run(line);
    }
}

void runFile(std::string path)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        throw std::runtime_error("Couldn't open " + path);
    }
    std::string code;
    auto start = in.tellg();
    in.seekg(std::ios::beg, std::ios::end);
    auto end = in.tellg();
    in.seekg(std::ios::beg, std::ios::beg);
    code.resize(end - start);
    in.read(&code[0], end - start);
    run(code);
}

int main(int argc, char* argv[])
{
    std::unique_ptr<Expr> expr{(new
        Binary(std::unique_ptr<Expr>(new Unary(Token{"-", TokenType::Minus, 0},
                                  std::unique_ptr<Expr>((new Literal(Token{"123", TokenType::Number, 0}))))),
               Token{"*", TokenType::Star, 0},
               std::unique_ptr<Expr>(new Grouping(std::unique_ptr<Expr>(new Literal(Token{"45.67", TokenType::Number, 0})))))
    )};
    ASTPrinter asp;
    asp.print(expr.get());
    if (argc > 2)
    {
        std::string execName = argv[0];
        auto loc = execName.rfind('/');
        if (loc == std::string::npos)
        {
            loc = execName.rfind('\\');
            if (loc == std::string::npos)
            {
                loc = 0;
            }
            else
            {
                ++loc;
            }
        }
        else
        {
            ++loc;
        }
        std::cout << "Usage: " + execName.substr(loc) + " [file]" << std::endl;
    }
    else if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        runPrompt();
    }
}

