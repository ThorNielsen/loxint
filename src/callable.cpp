#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    auto cl = closure;
    ScopeEnvironment se(cl, Environment::createNew(cl));
    for (size_t i = 0; i < args.size(); ++i)
    {
        se.env->createVar(params[i].lexeme, args[i]);
    }
    try
    {
        ScopeEnvironment guard(in.getEnv(), se.env);
        for (auto& stmt : statements->statements)
        {
            stmt->accept(in);
        }
    }
    catch (LoxObject lo)
    {
        return lo;
    }
    return LoxObject();
}
