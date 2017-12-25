#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::string trim(const std::string& s)
{
    size_t first = 0;
    while (first < s.size() && std::isspace(s[first])) ++first;
    size_t last = s.size() ? s.size() - 1 : 0;
    while (last > first && std::isspace(s[last])) --last;
    if (!std::isspace(s[last])) ++last;
    return s.substr(first, last-first);
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::stringstream ss(s);
    std::string reader;
    std::vector<std::string> res;
    while (std::getline(ss, reader, delim))
    {
        res.push_back(reader);
    }
    return res;
}

bool isPointer(const std::string& s)
{
    return s.size() && s.back() == '*';
}

bool isMoving(const std::string& s)
{
    return s.size() && s.back() == '\'';
}

std::string uppercase(std::string s)
{
    for (auto& c : s)
    {
        c = std::toupper(c);
    }
    return s;
}

std::string removePointer(const std::string& s)
{
    return s.substr(0, s.size()-1);
}

std::string argtype(const std::string& s)
{
    if (isPointer(s))
    {
        return "std::unique_ptr<" + removePointer(s) + ">&&";
    }
    if (isMoving(s))
    {
        return removePointer(s) + "&&";
    }
    return s;
}

std::string fieldtype(const std::string& s)
{
    if (isPointer(s))
    {
        return "std::unique_ptr<" + removePointer(s) + ">";
    }
    if (isMoving(s))
    {
        return removePointer(s);
    }
    return s;
}

void defineType(std::ostream& out, std::string baseName, std::string retType,
                std::string className, std::string fields)
{
    auto fieldList = split(fields, ',');
    for (auto& field : fieldList)
    {
        field = trim(field);
    }
    out << "class " << className << " : public " << baseName << '\n';
    out << "{\npublic:\n";
    out << "    " << className << "(";
    bool first = true;
    for (auto field : fieldList)
    {
        if (!first) out << ", ";
        first = false;
        auto type = split(field, ' ')[0];
        auto name = split(field, ' ')[1];
        out << argtype(type) << " " << name << "_";
    }
    out << ")\n";
    out << "    {\n";
    for (auto field : fieldList)
    {
        auto name = split(field, ' ')[1];
        if (isPointer(split(field, ' ')[0]) || isMoving(split(field, ' ')[0]))
        {
            out << "        " << name << " = std::move(" << name << "_);\n";
        }
        else
        {
            out << "        " << name << " = " << name << "_;\n";
        }
    }
    out << "    }\n\n";

    out << "    " << baseName << "RetType accept("
        << baseName << "Visitor& v) override\n";
    out << "    {\n";
    out << "        " << (retType != "void" ? "return " : "")
        << "v.visit" << className << "(*this);\n";
    out << "    }\n\n";

    for (auto field : fieldList)
    {
        auto type = split(field, ' ')[0];
        auto name = split(field, ' ')[1];
        out << "    " << fieldtype(type) << " " << name << ";\n";
    }
    out << "};\n\n";
}

void declareClasses(std::ostream& out,
                    const std::vector<std::string>& classNames)
{
    for (auto& cName : classNames)
    {
        out << "class " << cName << ";\n";
    }
    out << "\n";
}

void defineVisitor(std::ostream& out, std::string baseName, std::string retType,
                   const std::vector<std::string>& classNames)
{
    out << "class " << baseName << "Visitor\n";
    out << "{\n";
    out << "public:\n";
    for (auto& cName : classNames)
    {
        out << "    virtual " << baseName << "RetType visit" << cName
            << "(" << cName << "&) = 0;\n";
    }
    out << "};\n\n";
}

void createAst(std::ostream& out, std::string baseName, std::string retType,
               const std::vector<std::string>& types)
{
    out << "// Warning: This code is auto-generated and may be changed at any\n"
        << "// time by a script. Edits will be reverted.\n";
    out << "#ifndef " + uppercase(baseName) + "_HPP_INCLUDED\n";
    out << "#define " + uppercase(baseName) + "_HPP_INCLUDED\n";
    out << "#include \"token.hpp\"\n";
    out << "#include \"loxobject.hpp\"\n";
    out << "#include <memory>\n";
    out << "#include <vector>\n\n";
    out << "using " + baseName + "RetType = " << retType << ";\n\n";
    std::vector<std::string> classNames;
    for (auto& s : types)
    {
        classNames.push_back(trim(split(s, '|')[0]) + baseName);
    }
    declareClasses(out, classNames);
    defineVisitor(out, baseName, retType, classNames);
    out << "class " + baseName + "\n{\n";
    out << "public:\n";
    out << "    virtual " << retType << " accept("
        << baseName << "Visitor&) = 0;";
    out << "\n};\n\n";
    for (auto& s : types)
    {
        std::string className = trim(split(s, '|')[0]) + baseName;
        std::string fields = trim(split(s, '|')[1]);
        defineType(out, baseName, retType, className, fields);
    }
    out << "#endif // " + uppercase(baseName) + "_HPP_INCLUDED\n";
}

int main()
{
    std::vector<std::string> types;
    types.push_back("Assignment | Token name, Expr* val");
    types.push_back("Binary     | Expr* left, Token oper, Expr* right");
    types.push_back("Call       | Expr* callee, Token paren, std::vector<std::unique_ptr<Expr>>' args");
    types.push_back("Grouping   | Expr* expr");
    types.push_back("Literal    | LoxObject value");
    types.push_back("Logical    | Expr* left, Token oper, Expr* right");
    types.push_back("Unary      | Token oper, Expr* right");
    types.push_back("Variable   | Token name");
    std::ofstream out("../src/expr.hpp", std::ios::trunc);
    if (!out.is_open())
    {
        std::cerr << "Could not open file for writing!\n";
        return 1;
    }
    createAst(out, "Expr", "LoxObject", types);
    types.clear();
    types.push_back("Block      | std::vector<std::unique_ptr<Stmt>>' statements");
    types.push_back("Expression | Expr* expr");
    types.push_back("If         | Expr* cond, Stmt* thenBranch, Stmt* elseBranch");
    types.push_back("Print      | Expr* expr");
    types.push_back("Variable   | Token name, Expr* init");
    types.push_back("While      | Expr* cond, Stmt* statement");
    out.close();
    out.open("../src/stmt.hpp", std::ios::trunc);
    if (!out.is_open())
    {
        std::cerr << "Could not open file for writing!\n";
        return 1;
    }
    createAst(out, "Stmt", "void", types);
}
