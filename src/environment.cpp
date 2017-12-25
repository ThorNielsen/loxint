#include "environment.hpp"
#include "callable.hpp"

Environment::Environment()
{
    pushScope();
    createVar("clock", LoxCallable(new TimeFunction()));
}
