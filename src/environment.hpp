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

    static PEnvironment createNew(PEnvironment encl)
    {
        return std::make_shared<Environment>(encl);
    }
    static PEnvironment copy(PEnvironment env, PEnvironment encl)
    {
        auto newEnv = createNew(encl);
        newEnv->m_vars = env->m_vars;
        return newEnv;
    }
    bool exists(std::string name)
    {
        if (m_vars.find(name) != m_vars.end()) return true;
        if (enclosing != nullptr) return enclosing->exists(name);
        return false;
    }
    size_t getDist(std::string name)
    {
        auto var = m_vars.find(name);
        if (var != m_vars.end())
        {
            return 0;
        }
        if (enclosing != nullptr) return 1 + enclosing->getDist(name);
        throw LoxError("Unable to get distance of " + name + ".");
    }
    LoxObject& get(size_t dist, std::string name)
    {
        if (!dist) return get(name);
        return enclosing->get(dist-1, name);
    }
    LoxObject& get(std::string name)
    {
        auto var = m_vars.find(name);
        if (var != m_vars.end())
        {
            return var->second;
        }
        if (enclosing != nullptr) return enclosing->get(name);
        throw LoxError("'" + name + "' was not declared in this scope.");
    }
    void assign(size_t dist, std::string name, LoxObject lo)
    {
        if (!dist) assign(name, lo);
        else assign(dist-1, name, lo);
    }
    void assign(std::string name, LoxObject lo)
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

