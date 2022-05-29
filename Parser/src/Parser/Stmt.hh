#include "../../../Lexer/src/Lexer/Token.hh"
#include "Expr.hh"
#include <variant>
#include <vector>

using namespace ktpp::lexer;

namespace ktpp::parser {
    class Stmt {};
    class Scope : public Stmt {
    public:
        std::vector<Stmt> statements;
        Scope(std::vector<Stmt> statements) : statements(statements) {}
    };
    class Expression : public Stmt {
    public:
        Expr expr;
        Expression(Expr expr) : expr(expr) {}
    };
    class If : public Stmt {
    public:
        Expr condition;
        Stmt thenBranch;
        std::optional<Stmt> elseBranch;
        If(Expr condition, Stmt thenBranch, std::optional<Stmt> elseBranch)
            : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
    };
    class Return : public Stmt {
    public:
        std::optional<Expr> value;
        Token keyword;
        Return(Token keyword, std::optional<Expr> value)
            : keyword(keyword), value(value) {}
    };
    class Var : public Stmt {
    public:
        Token name;
        std::optional<Expr> initializer;
        std::optional<Type> type;
        bool isMut;
        Var(Token name, std::optional<Expr> initializer, std::optional<Type> type, bool isMut = false)
            : name(name), initializer(initializer), type(type), isMut(isMut) {}
    };
    class While : public Stmt {
    public:
        Expr condition;
        Stmt statement;
        bool isDoWhile;
        While(Expr condition, Stmt statement, bool isDoWhile = false)
            : condition(condition), statement(statement), isDoWhile(isDoWhile) {}
    };
    class For : public Stmt {
    public:
        std::optional<Stmt> init;
        Expr condition;
        Expr increment;
        Stmt statement;
        For(std::optional<Stmt> init, Expr condition, Expr increment, Stmt statement)
            : init(init), condition(condition), increment(increment), statement(statement) {}
    };
    class ForEach : public Stmt {
    public:
        Var init;
        Token colon;
        Stmt statement;
        Expr iterator;
        ForEach(Var init, Token colon, Expr iterator, Stmt statement)
            : init(init), colon(colon), iterator(iterator), statement(statement) {}
    };
    class Break : public Stmt {
    public:
        Token keyword;
        Break(Token keyword)
            : keyword(keyword) {}
    };
    class Continue : public Stmt {
    public:
        Token keyword;
        Continue(Token keyword)
            : keyword(keyword) {}
    };
    struct Param {
        Token name;
        Type type;
        Param(Token name, Type type) : name(name), type(type) {}
    };
    class Function : public Stmt {
    public:
        Token name;
        std::optional<Type> type;
        std::optional<std::vector<Generic>> generics;
        std::optional<std::vector<Param>> params;
        Stmt body;
        Function(Token name, std::optional<std::vector<Generic>> generics, std::optional<std::vector<Param>> params, Scope body, std::optional<Type> type)
            : name(name), params(params), body(body), generics(generics), type(type) {}
    };
    class Class : public Stmt {
    public:
        Token name;
        std::vector<Function> methods;
        std::optional<std::vector<Generic>> generics;
        std::optional<Expr> superclass;
        Class(Token name, std::vector<Var> fields, std::vector<Function> methods,
            std::optional<Expr> superclass, std::optional<std::vector<Generic>> generics)
            : name(name), methods(methods), superclass(superclass), generics(generics) {}
    };
    template<typename T>
    struct SwitchCase {
        Expr value;
        T x;
        Token op;
        SwitchCase(Expr value, T x, Token op) : value(value), x(x), op(op) {}
    };
    template<typename T>
    class Switch : public Stmt {
    public:
        Expr condition;
        std::vector<SwitchCase<T>> cases;
        std::optional<Stmt> defaultCase;
        Switch(Expr condition, std::vector<SwitchCase<T>> cases, std::optional<Stmt> defaultCase)
            : condition(condition), cases(cases), defaultCase(defaultCase) {}
    };
}