#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "lexer.hpp"
#include "resolver.hpp"

#include "astprinter.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

int run(Interpreter& interpreter, std::string source, bool repl = false)
{
    try
    {
        Parser p;
        Lexer lex;
        auto stmts = p.parse(lex.scanTokens(source), repl);
        ASTPrinter printer;
        for (auto& stmt : stmts)
        {
            printer.print(stmt.get());
        }
        if (p.hadError())
        {
            if (p.continuable())
            {
                return 1;
            }
            return 2;
        }
        Resolver r;
        if (!r.resolve(stmts, interpreter))
        {
            return 4;
        }
        interpreter.interpret(std::move(stmts));
        return 0;
    }
    catch (LoxError err)
    {
        if (repl) return 3;
        throw;
    }
}

bool isspace(std::string s)
{
    for (auto& c : s)
    {
        switch(c)
        {
        case '\n': case '\r': case '\t': case ' ':
            break;
        default:
            return false;
        }
    }
    return true;
}

void runPrompt()
{
    Interpreter interpreter;
    std::string prevCode = "";
    while (std::cin.good())
    {
        if (!isspace(prevCode)) std::cout << ". ";
        else                    std::cout << "> ";
        std::string line;
        std::getline(std::cin, line, '\n');

        // If the user wants to stop appending code to previous code, they enter
        // a blank line (one consisting entirely of whitespace) in which case we
        // want to show them all errors in the previous code.
        if (isspace(line) && !isspace(prevCode))
        {
            run(interpreter, prevCode, false);
            prevCode = "";
            continue;
        }
        if (run(interpreter, prevCode+line, true) == 1)
        {
            prevCode += (prevCode.empty() ? "" : "\n") + line;
        }
        else
        {
            prevCode = "";
        }
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
    return run(interpreter, code) == 0;
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

