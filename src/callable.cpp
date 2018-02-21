#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction(FunctionStmt* stmt, Interpreter* in,
                         PEnvironment enclosing, bool init)
{
    //std::cerr << "func()\n";
    interpreter = in;
    recCount = 0;
    fname = stmt->name;
    params = stmt->params;
    statements = stmt->statements.get();
    initialiser = init;

    interpreter->registerFunction(this, enclosing->copy());
}

LoxFunction::LoxFunction(LoxFunction& other, Interpreter* in,
                         PEnvironment enclosing)
{
    //std::cerr << "func(func)\n";
    interpreter = in;
    recCount = 0;
    fname = other.fname;
    params = other.params;
    statements = other.statements;
    initialiser = other.initialiser;

    interpreter->registerFunction(this, enclosing);
}

LoxFunction::~LoxFunction()
{
    //std::cerr << "~func()\n";
    interpreter->deleteFunction(this);
}

LoxObject LoxFunction::operator()(Interpreter& in, Arguments args,
                                  std::shared_ptr<Callable>)
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

LoxObject LoxClass::operator()(Interpreter& in, Arguments args,
                               std::shared_ptr<Callable>)
{
    auto instance = std::make_shared<LoxInstance>(*this);
    if (methods.find("init") != methods.end())
    {
        instance->get({"init", TokenType::Identifier, 0}, instance)(in, args);
    }
    return instance;
}
