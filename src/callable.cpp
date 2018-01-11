#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    auto cl = closure;
    ScopeEnvironment se(in.getEnv(), Environment::createNew(cl));
    for (size_t i = 0; i < args.size(); ++i)
    {
        se.env->assign(params[i].lexeme, args[i]);
    }
    try
    {
        for (auto& stmt : statements->statements)
        {
            stmt->accept(in);
        }
    }
    catch (LoxObject lo)
    {
        return lo;
    }
    if (initialiser) return closure->get(0, "this");
    return LoxObject();
}

LoxObject LoxClass::operator()(Interpreter& in, Arguments args)
{
    auto instance = std::make_shared<LoxInstance>(*this);
    if (methods.find("init") != methods.end())
    {
        instance->get({"init", TokenType::Identifier, 0}, instance)(in, args);
    }
    return instance;
}
