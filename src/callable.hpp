#ifndef CALLABLE_HPP_INCLUDED
#define CALLABLE_HPP_INCLUDED

#include "loxobject.hpp"
#include "stmt.hpp"
#include "environment.hpp"

#include <chrono>
#include <map>
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
        double t = std::chrono::duration<double>(Clock::now() - begin).count();
        return LoxObject(t);
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
    LoxFunction(FunctionStmt* stmt, Interpreter* intp,
                PEnvironment enclosing, bool init = false);
    LoxFunction(LoxFunction& other, PEnvironment enclosing);

    ~LoxFunction();


    size_t arity() const override { return params.size(); }
    LoxObject operator()(Interpreter& in, Arguments args) override;
    std::string name() const override { return fname.lexeme; }
private:
    friend class LoxClass;
    friend class LoxInstance;
    PEnvironment closure = nullptr;
    Interpreter* interpreter;
    Token fname;
    std::vector<Token> params;
    BlockStmt* statements;
    bool initialiser;
};

class LoxInstance;

class LoxClass
{
public:
    LoxClass(ClassStmt* stmt, Interpreter* intp, LoxClass* superclass,
             PEnvironment enclosing);
    ~LoxClass()
    {
        ///std::cerr << "~LoxClass() -- " << this << "\n";
    }
    size_t arity() const
    {
        auto method = methods.find("init");
        if (method != methods.end())
        {
            return method->second.function->arity();
        }
        return 0;
    }
    LoxObject operator()(Interpreter& in, Arguments args);
    std::string name() const { return cname.lexeme; }
    LoxObject function(Token pname, LoxInstance* instance);
private:
    friend class LoxInstance;

    Interpreter* interpreter;
    LoxClass* super;
    std::map<std::string, LoxObject> methods;
    Token cname;
};

class LoxInstance
{
public:
    LoxInstance(LoxClass& lc)
        : properties{}, cname(lc.cname), classy{&lc}
    {
        ///std::cerr << "LoxInstance() -- " << this << "\n";
    }
    ~LoxInstance()
    {
        ///std::cerr << "~LoxInstance() -- " << this << "\n";
    }
    std::string name()
    {
        return cname.lexeme;
    }
    LoxObject get(Token pname)
    {
        auto var = properties.find(pname.lexeme);
        if (var != properties.end())
        {
            return var->second;
        }
        return classy->function(pname, this);
    }
    LoxObject set(Token pname, LoxObject value)
    {
        return properties[pname.lexeme] = value;
    }
private:
    std::map<std::string, LoxObject> properties;
    Token cname;
    LoxClass* classy;
};

#endif // CALLABLE_HPP_INCLUDED

