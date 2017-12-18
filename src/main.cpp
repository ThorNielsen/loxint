#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "lexer.hpp"


void run(std::string source)
{
    Lexer lex(source);
    for (auto tok : lex.scanTokens())
    {
        std::cout << tok << ' ';
    }
    std::cout << '\n';
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

