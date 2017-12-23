#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include "loxobject.hpp"
#include <map>
#include <string>
#include <vector>

class Environment
{
public:
    Environment() {}
    ~Environment() {}

    bool exists(std::string name)
    {
        return m_vars.find(name) == m_vars.end();
    }
    LoxObject& getVar(std::string name)
    {
        auto var = m_vars.find(name);
        if (var == m_vars.end())
        {
            throw LoxError("'" + name + "' was not declared.");
        }
        return var->second;
    }
    void createVar(std::string name, LoxObject lo)
    {
        m_vars[name] = lo;
    }
private:
    std::map<std::string, LoxObject> m_vars;
};

#endif // ENVIRONMENT_HPP_INCLUDED

