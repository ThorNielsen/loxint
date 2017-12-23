#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include "loxobject.hpp"
#include <map>
#include <string>
#include <vector>

class Environment
{
public:
    Environment()
    {
        pushScope();
    }
    ~Environment() {}

    bool exists(std::string name)
    {
        for (auto vars = m_vars.rbegin(); vars != m_vars.rend(); ++vars)
        {
            if (vars->find(name) != vars->end()) return true;
        }
        return false;
    }
    LoxObject& getVar(std::string name)
    {
        for (auto vars = m_vars.rbegin(); vars != m_vars.rend(); ++vars)
        {
            auto var = vars->find(name);
            if (var != vars->end())
            {
                return var->second;
            }
        }
        throw LoxError("'" + name + "' was not declared.");
    }
    void createVar(std::string name, LoxObject lo)
    {
        m_vars.back()[name] = lo;
    }
    void pushScope()
    {
        m_vars.push_back({});
    }
    void popScope()
    {
        m_vars.pop_back();
    }
private:
    std::vector<std::map<std::string, LoxObject>> m_vars;
};

class ScopeGuard
{
public:
    ScopeGuard(Environment& env) : m_env(env) { env.pushScope(); }
    ~ScopeGuard() { m_env.popScope(); }
private:
    Environment& m_env;
};

#endif // ENVIRONMENT_HPP_INCLUDED

