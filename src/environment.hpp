#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include "loxobject.hpp"
#include <map>
#include <string>
#include <vector>

class Environment
{
public:
    Environment() { }
    ~Environment() { }

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
    Environment* enclosing;
private:
    std::map<std::string, LoxObject> m_vars;
};

class ScopeEnvironment
{
public:
    ScopeEnvironment(Environment*& prev) : m_enc(prev)
    {
        newEnv = new Environment;
        newEnv->enclosing = prev;
        prev = newEnv;
    }
    ~ScopeEnvironment()
    {
        m_enc = m_enc->enclosing;
        delete newEnv;
    }
    Environment* newEnv;
private:
    Environment*& m_enc;
};

#endif // ENVIRONMENT_HPP_INCLUDED

