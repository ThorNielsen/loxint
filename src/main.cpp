#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "lexer.hpp"

#include "astprinter.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

bool run(Interpreter& interpreter, std::string source)
{
    try
    {
        Lexer lex;
        Parser p;
        auto stmts = p.parse(lex.scanTokens(source));
        interpreter.interpret(stmts);
        return true;
    }
    catch (LoxError& err)
    {
        std::cerr << "Error: " << err.what() << "\n";
        return false;
    }
}

void runPrompt()
{
    Interpreter interpreter;
    while (std::cin.good())
    {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line, '\n');
        run(interpreter, line);
    }
}

bool runFile(std::string path)
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
    Interpreter interpreter;
    return run(interpreter, code);
}

int main(int argc, char* argv[])
{
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
        return 1;
    }
    else if (argc == 2)
    {
        return runFile(argv[1]) ? 0 : 'm'^'a'^'g'^'i'^'c';
    }
    else
    {
        runPrompt();
        return 0;
    }
}

