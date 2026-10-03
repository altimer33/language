#include "tokens/lexer.hpp"
#include <optional>

namespace compiler::parser {
    class Parser;

    class NamedNode {
    public:
        // virtual std::string name();

        virtual ~NamedNode() = default;
    };

    class Linkable {
    public:
        // virtual Failable<void> link();
        // bool linked() const;
        
        virtual ~Linkable() = default;
    };

    class Parent : virtual public Linkable {
    //     Parent *parent;

    // public:
    //     Failable<NamedNode *> resolveIdentifier(lexer::Token &token);
    };

    class Declaration : virtual public NamedNode, virtual public Linkable {
    public:
        virtual std::string string() const = 0;
        static Failable<Declaration *> parse(Parser &parser);
    };

    class TypeDeclaration;
    
    class Type : virtual public Linkable {
        TypeDeclaration *_source;
    
    public:
        Type(TypeDeclaration *source);
        const TypeDeclaration *source() const;

        std::string string() const;
        
        // bool linked() const;
        static Failable<Type> parse(Parser &parser);
    };

    class TypeDeclaration : virtual public Declaration {
    public:
        virtual std::string refString() const = 0;
    };

    // class UnlinkedType : virtual TypeDeclaration {
    //     std::vector<lexer::Token> _nameTokens;
    
    // public:
    //     UnlinkedType(std::vector<lexer::Token> &&tokens);
    //     UnlinkedType(lexer::Token tokens);
    //     bool linked() const;
    // };

    class PrimitiveType : virtual public TypeDeclaration {
    private:
        const std::string _name;
        const size_t _size;
        const size_t _align;

    public:
        std::string string() const;
        std::string refString() const;

        PrimitiveType(const std::string name, size_t size);
        PrimitiveType(const std::string name, size_t size, size_t allign);
    };

    extern PrimitiveType _void;
    extern PrimitiveType _bool;
    extern PrimitiveType _char;
    extern PrimitiveType _int;
    extern PrimitiveType _long;
    extern PrimitiveType _float;
    extern PrimitiveType _double;

    class Field : virtual public Declaration {
    private:
        const lexer::Token _name;
        Type _type;

    public:
        const lexer::TokenValue name() const;
        const lexer::Token &nameToken() const; 
        const Type type() const;
        
        std::string string() const;

        Field(Type type, lexer::Token name);
    };

    class Variable : public Field {
        // Expression *initialValue;
    public:
        Variable(Type type, lexer::Token name);
        std::string string() const;

        static Failable<Variable *> parse(Parser &parser);
    };

    class Parameter : public Field {
    public:
        Parameter(Type type, lexer::Token name);
        std::string string() const;
        static Failable<Parameter *> parse(Parser &parser);
    };

    // class Expression {
        
    // };

    class Function : virtual public Declaration {
    private:
        const lexer::Token _name;
        Type _type;
        std::vector<Parameter *> _parameters;
    
    public:
        Function(lexer::Token name, Type type, std::vector<Parameter *> &&parameters/*, std::vector<Statement *> &&statements*/);

        lexer::TokenValue name() const;
        const lexer::Token &nameToken() const;
        const Type &type() const;
        const std::vector<Parameter *> &parameters() const;

        std::string string() const;

        static Failable<Function *> parse(Parser &parser, Type &type, lexer::Token &identifier);
    };

    class Parser {
    private:
        lexer::Lexer &_lexer;
        std::vector<Declaration *> fileDeclarations = {};
        std::optional<Failable<lexer::Token>> storedToken = std::nullopt;
        bool parsed = false;

    public:
        Failable<void> next();

        Failable<lexer::Token> nextToken();
        Failable<void> skipToken(lexer::TokenType type);
        Failable<bool> skipIfToken(lexer::TokenType type);
        Failable<lexer::Token> peekToken();
        Failable<lexer::TokenType> peekType();
        Failable<lexer::Token> getToken(lexer::TokenType type);
        bool peekTypeIs(lexer::TokenType type);
        bool peekTypeIs(bool (&classification)[lexer::countoftypes]);
        Failable<void> parseFile();
        bool done();

        std::string string() const;

        Parser(lexer::Lexer &lexer) :
            _lexer{lexer}
        {}
    };
}