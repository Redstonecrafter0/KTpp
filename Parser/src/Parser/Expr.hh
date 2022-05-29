#include <vector>
#include <optional>

namespace ktpp::parser {
    class Expr {};
    class Literal : public Expr {
    public:
        std::any value;
        Literal(std::any value) : value(value) {}
    };
    class Binary : public Expr {
    public:
        Expr left, right;
        Token op;
        Binary(Expr left, Token op, Expr right) : left(left), op(op), right(right) {}
    };
    class Unary : public Expr {
    public:
        Expr left;
        Token op;
        Unary(Expr left, Token op) : left(left), op(op) {}
    };
    class Grouping : public Expr {
    public:
        Expr expr;
        Grouping(Expr expr) : expr(expr) {}
    };
    class Variable : public Expr {
    public:
        Token name;
        Variable(Token name) : name(name) {}
    };
    class Assign : public Expr {
    public:
        Token name;
        Expr value;
        Assign(Token name, Expr value) : name(name), value(value) {}
    };
    class Call : public Expr {
    public:
        Expr callee;
        Token paren;
        std::vector<Expr> args;
        std::vector<Token> generics;
        Call(Expr callee, Token paren, std::vector<Expr> args, std::vector<Token> generics)
            : callee(callee), paren(paren), args(args), generics(generics) {}
    };
    class Get : public Expr {
    public:
        Expr object;
        Token name;
        Get(Expr object, Token name) : object(object), name(name) {}
    };
    class Set : public Expr {
    public:
        Expr object;
        Token name;
        Expr value;
        Set(Expr object, Token name, Expr value) : object(object), name(name), value(value) {}
    };
    class Ternary : public Expr {
    public:
        Expr condition, ifTrue, ifFalse;
        Token questionM;
        Ternary(Expr condition, Expr ifTrue, Expr ifFalse, Token questionM)
            : condition(condition), ifTrue(ifTrue), ifFalse(ifFalse), questionM(questionM) {}
    };
    class This : public Expr {
    public:
        Token keyword;
        This(Token keyword) : keyword(keyword) {}
    };
    class Array : public Expr {
    public:
        Token bracket;
        std::vector<Expr> values;
        Array(Token bracket, std::vector<Expr> values) : bracket(bracket), values(values) {}
    };
    class IndexSignature : public Expr {
    public:
        Expr array, index;
        Token bracket;
        IndexSignature(Expr array, Expr index, Token bracket) : array(array), index(index), bracket(bracket) {}
    };
    class Generic : public Expr {
    public:
        Token LOp;
        std::map<Token, std::optional<Type>> type;
        Generic(Token LOp, std::map<Token, std::optional<Type>> type) : LOp(LOp), type(type) {}
    };
    class Type : public Expr {
    public:
        Token identifier;
        bool isGeneric;
        std::optional<std::vector<Generic>> generics;
        std::optional<Type> extends;
        Type(Token identifier, std::optional<std::vector<Generic>> generics, std::optional<Type> extends)
            : identifier(identifier), isGeneric(generics.has_value()), generics(generics), extends(extends) { }
    };
    class Lambda : public Expr {
    public:
        Token keyword;
        bool isGeneric;
        std::optional<std::vector<Generic>> generics;
        Expr expr;
        Lambda(Token keyword, std::optional<std::vector<Generic>> generics, std::optional<std::vector<Param>>, std::optional<Type> type, Expr expr)
            : keyword(keyword), expr(expr), isGeneric(generics.has_value()), generics(generics) { }
    };
}