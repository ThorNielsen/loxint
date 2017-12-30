#include "environment.hpp"
#include "callable.hpp"
#include <memory>

Environment::Environment()
{
    pushScope();
    auto clock = LoxCallable(new TimeFunction());
    createVar(clock->name(), clock);
}
