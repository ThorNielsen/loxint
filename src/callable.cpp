#include "callable.hpp"

#include "environment.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction(FunctionStmt* stmt, Interpreter* intp,
                         PEnvironment enclosing, bool init)
{
    ///std::cerr << "LoxFunction() -- " << this << "\n";
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
    ///std::cerr << "LoxFunction(func) -- " << this << "\n";
    fname = other.fname;
    params = other.params;
    statements = other.statements;
    initialiser = other.initialiser;
    interpreter = other.interpreter;

    interpreter->registerFunction(this, enclosing);
}

LoxFunction::~LoxFunction()
{
    ///std::cerr << "~LoxFunction() -- " << this << "\n";
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
    if (&in != interpreter)
    {
        throw std::runtime_error("Class constructed in different interpreter.");
    }
    if (methods.find("init") != methods.end())
    {
        return LoxObject(interpreter->createInstance(this, args),
                         interpreter);
    }
    return LoxObject(interpreter->createInstance(this), interpreter);
}

LoxClass::LoxClass(ClassStmt* stmt, Interpreter* intp, LoxClass* superclass,
                   PEnvironment enclosing)
{
    ///std::cerr << "LoxClass() -- " << this << "\n";
    if (superclass == this)
    {
        throw std::logic_error("Trying to make a class a subclass of itself");
    }
    super = superclass;
    cname = stmt->name;
    interpreter = intp;
    for (auto& f : stmt->methods)
    {
        auto* func = interpreter->createFunction(f.get(), enclosing,
                                                 f->name.lexeme == "init");
        methods[f->name.lexeme] = LoxObject(func, interpreter);
    }
}

LoxObject LoxClass::function(Token pname, LoxInstance* instance)
{
    auto func = methods.find(pname.lexeme);
    if (func != methods.end())
    {
        LoxFunction* fun = static_cast<LoxFunction*>(func->second.function);
        PEnvironment env =
            std::make_shared<Environment>(fun->closure);
        env->assign("this", LoxObject(instance));

        return LoxObject(interpreter->createFunction(fun, env), interpreter);
    }
    if (super) return super->function(pname, instance);
    throw LoxError("Could not find " + pname.lexeme + ".");
}
