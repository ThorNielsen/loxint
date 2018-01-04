#ifndef CALLABLE_HPP_INCLUDED
#define CALLABLE_HPP_INCLUDED

#include "loxobject.hpp"
#include "stmt.hpp"

#include <chrono>
#include <memory>

using Arguments = std::vector<LoxObject>;

class Callable
{
public:
    virtual ~Callable() {}
    virtual size_t arity() const = 0;
    virtual LoxObject operator()(Interpreter&, Arguments args) = 0;
    virtual std::string name() const = 0;
};

class TimeFunction : public Callable
{
public:
    using Clock = std::chrono::high_resolution_clock;
    TimeFunction()
    {
        begin = Clock::now();
    }
    ~TimeFunction() {}
    size_t arity() const override { return 0; }
    LoxObject operator()(Interpreter&, Arguments)
    {
        return std::chrono::duration<double>(Clock::now() - begin).count();
    }
    std::string name() const override
    {
        return "clock";
    }
private:
    decltype(Clock::now()) begin;
};

class LoxFunction : public Callable
{
public:
    // Warning: This stores a non-owning pointer to the function declaration
    // statements. These must continue to be available for as long as this
    // object lives.
    LoxFunction(Stmt* stmt)
    {
        auto* fs = static_cast<FunctionStmt*>(stmt);
        fname = fs->name;
        params = fs->params;
        statements = fs->statements.get();
    }

    size_t arity() const override { return params.size(); }
    LoxObject operator()(Interpreter& in, Arguments args) override;
    std::string name() const override { return fname.lexeme; }
private:
    Token fname;
    std::vector<Token> params;
    BlockStmt* statements;
};

#endif // CALLABLE_HPP_INCLUDED

