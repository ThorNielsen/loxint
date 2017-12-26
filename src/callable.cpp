#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    Environment& env = in.getEnv();
    ScopeGuard guard(env);
    for (size_t i = 0; i < args.size(); ++i)
    {
        env.createVar(params[i].lexeme, args[i]);
    }
    in.visitBlockStmt(*statements);
    return LoxObject();
}
