#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction(FunctionStmt* stmt, Interpreter* intp,
                         PEnvironment enclosing, bool init)
{
    std::cerr << "func() -- " << this << "\n";
    interpreter = intp;
    fname = stmt->name;
    params = stmt->params;
    statements = stmt->statements.get();
    initialiser = init;

    interpreter->registerFunction(this, enclosing->copy());
}

LoxFunction::LoxFunction(LoxFunction& other,
                         PEnvironment enclosing)
{
    std::cerr << "func(func) -- " << this << "\n";
    fname = other.fname;
    params = other.params;
    statements = other.statements;
    initialiser = other.initialiser;
    interpreter = other.interpreter;

    interpreter->registerFunction(this, enclosing);
}

LoxFunction::~LoxFunction()
{
    std::cerr << "~func() -- " << this << "\n";
    interpreter->deleteFunction(this);
}

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args)
{
    auto cl = interpreter->getFunctionEnvironment(this);
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
