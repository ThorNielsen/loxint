#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include "loxobject.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Environment;
using PEnvironment = std::shared_ptr<Environment>;

class Environment
{
public:
    Environment(PEnvironment encl) : enclosing{encl} { }
    ~Environment() { }

    static PEnvironment createNew(PEnvironment old)
    {
        return std::make_shared<Environment>(old);
    }
    bool exists(std::string name)
    {
        if (m_vars.find(name) != m_vars.end()) return true;
        if (enclosing != nullptr) return enclosing->exists(name);
        return false;
    }
    LoxObject& getVar(std::string name)
    {
        auto var = m_vars.find(name);
        if (var != m_vars.end())
        {
            return var->second;
        }
        if (enclosing != nullptr) return enclosing->getVar(name);
        throw LoxError("'" + name + "' was not declared.");
    }
    void createVar(std::string name, LoxObject lo)
    {
        m_vars[name] = lo;
    }
    const PEnvironment enclosing;
private:
    std::map<std::string, LoxObject> m_vars;
};

class ScopeEnvironment
{
public:
    ScopeEnvironment(PEnvironment& enclosing,
                     PEnvironment newEnv)
                     : env{newEnv}, prev{enclosing}, var(enclosing)
    {
        var = env;
    }
    ~ScopeEnvironment()
    {
        var = prev;
    }
    PEnvironment env;
private:
    PEnvironment prev;
    PEnvironment& var;
};

#endif // ENVIRONMENT_HPP_INCLUDED

