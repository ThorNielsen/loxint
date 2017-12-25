#ifndef CALLABLE_HPP_INCLUDED
#define CALLABLE_HPP_INCLUDED

#include "loxobject.hpp"

#include <chrono>

using Arguments = std::vector<LoxObject>;

class Callable
{
public:
    virtual ~Callable() {}
    virtual size_t arity() const = 0;
    virtual LoxObject operator()(Interpreter&, Arguments args) = 0;
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
private:
    decltype(Clock::now()) begin;
};

#endif // CALLABLE_HPP_INCLUDED

