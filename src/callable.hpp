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
    LoxFunction(FunctionStmt* stmt, PEnvironment enclosing, bool init = false)
    {
        fname = stmt->name;
        params = stmt->params;
        statements = stmt->statements.get();
        closure = enclosing;
        initialiser = init;
    }
    LoxFunction(LoxFunction& other, PEnvironment enclosing)
    {
        fname = other.fname;
        params = other.params;
        statements = other.statements;
        closure = enclosing;
        initialiser = other.initialiser;
    }

    ~LoxFunction() { }


    size_t arity() const override { return params.size(); }
    LoxObject operator()(Interpreter& in, Arguments args) override;
    std::string name() const override { return fname.lexeme; }
private:
    friend class LoxClass;
    friend class LoxInstance;
    PEnvironment closure;
    Token fname;
    std::vector<Token> params;
    BlockStmt* statements;
    bool initialiser;
};

class LoxInstance;

class LoxClass : public Callable
{
public:
    LoxClass(ClassStmt* stmt, std::shared_ptr<LoxClass> superclass,
             PEnvironment enclosing)
    {
        instances = 0;
        if (superclass.get() == this)
        {
            throw std::logic_error("Trying to make a class a subclass of itself");
        }
        super = superclass;
        cname = stmt->name;
        for (auto& f : stmt->methods)
        {
            auto func = std::make_shared<LoxFunction>(f.get(), enclosing,
                                                      f->name.lexeme == "init");
            methods[f->name.lexeme] = func;
        }
    }
    ~LoxClass()
    {
        if (instances)
        {
            std::cerr << instances << " instances of " << name() << " left!\n";
            throw std::logic_error("Deleted class before its instances.");
        }
    }
    size_t arity() const override
    {
        auto method = methods.find("init");
        if (method != methods.end())
        {
            return method->second->arity();
        }
        return 0;
    }
    LoxObject operator()(Interpreter& in, Arguments args) override;
    std::string name() const override { return cname.lexeme; }
    LoxObject function(Token pname, std::shared_ptr<LoxInstance> thisInstance)
    {
        auto func = methods.find(pname.lexeme);
        if (func != methods.end())
        {
            PEnvironment env = std::make_shared<Environment>(func->second->closure);
            env->assign("this", thisInstance);
            return LoxObject(std::make_shared<LoxFunction>(*func->second, env));
        }
        if (super) return super->function(pname, thisInstance);
        throw LoxError("Could not find " + pname.lexeme + ".");
    }
private:
    friend class LoxInstance;


    void registerInstance() { ++instances; }
    void removeInstance() { --instances; }
    std::shared_ptr<LoxClass> super;
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    Token cname;
    size_t instances;
};

class LoxInstance
{
public:
    LoxInstance(LoxClass& lc)
        : properties{}, cname(lc.cname), classy{&lc}
    {
        classy->registerInstance();
    }
    ~LoxInstance()
    {
        classy->removeInstance();
    }
    std::string name()
    {
        return cname.lexeme;
    }
    LoxObject get(Token pname, std::shared_ptr<LoxInstance> thisInstance)
    {
        auto var = properties.find(pname.lexeme);
        if (var != properties.end())
        {
            return var->second;
        }
        return classy->function(pname, thisInstance);
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

