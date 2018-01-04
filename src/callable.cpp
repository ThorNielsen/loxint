#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    Environment* env = &in.getEnv();
    ScopeEnvironment newEnv(env);
    for (size_t i = 0; i < args.size(); ++i)
    {
        env->createVar(params[i].lexeme, args[i]);
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
    return LoxObject();
}
