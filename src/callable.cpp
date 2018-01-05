#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    auto cl = closure;
    ScopeEnvironment se(in.getEnv(), Environment::createNew(cl));
    //ScopeEnvironment se(in.getEnv(), Environment::copy(cl, in.getEnv()));
    for (size_t i = 0; i < args.size(); ++i)
    {
        se.env->assign(params[i].lexeme, args[i]);
    }
    //se.env->dump();
    try
    {
        //statements->accept(in);

        //ScopeEnvironment guard(in.getEnv(), Environment::createNew(in.getEnv()));
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
