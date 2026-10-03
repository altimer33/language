#include <format>
#include <string>
#include <ranges>

#include "declarations.hpp"
#include "exceptions.hpp"
#include "logging/log.hpp"

namespace compiler::parser {
    PrimitiveType _void = PrimitiveType("void", 0);
    PrimitiveType _bool = PrimitiveType("bool", 1);
    PrimitiveType _char = PrimitiveType("char", 1);
    PrimitiveType _int = PrimitiveType("int", 4);
    PrimitiveType _long = PrimitiveType("long", 8);
    PrimitiveType _float = PrimitiveType("float", 4);
    PrimitiveType _double = PrimitiveType("double", 8);

    Failable<Declaration *> Declaration::parse(Parser &parser) {
        logger::log("Parsing Declaration");
        Failable<Type> type = Type::parse(parser);
        if (!type.valid()) {
            return static_cast<Failable<Declaration *>>(type);
        }
        Failable<lexer::Token> identifier = parser.nextToken();
        if (!identifier.valid()) {
            return static_cast<Failable<Declaration *>>(identifier);
        }
        if (parser.peekTypeIs(lexer::TokenType::PUNC_SEMICOLON)) {
            parser.next();
            return static_cast<Failable<Declaration *>>(new Variable(*type, *identifier));
        } else if (parser.peekTypeIs(lexer::TokenType::PUNC_LPAREN)) {
            return static_cast<Failable<Declaration *>>(Function::parse(parser, *type, *identifier));
        } else {
            Failable<lexer::Token> token = parser.nextToken();
            if (!token.valid()) {
                return static_cast<Failable<Declaration *>>(token);
            } else {
                return UnexpectedTokenError(*token);
            }
        }
    }

    Type::Type(TypeDeclaration *source) :
        _source{source}
    {}

    const TypeDeclaration *Type::source() const {
        return _source;
    }

    std::string Type::string() const {
        return std::format("Type({})", source()->refString());
    }

    Failable<Type> Type::parse(Parser &parser) {
        logger::log("Parsing Type");
        Failable<lexer::Token> token = parser.nextToken();
        if (!token.valid()) return (Failable<Type>) token;
        std::string valueString = *token->sourceString();
        if (valueString == "void") {
            return Type((TypeDeclaration *) &_void);
        } else if (valueString == "int") {
            return Type((TypeDeclaration *) &_int);
        } else if (valueString == "long") {
            return Type((TypeDeclaration *) &_long);
        } else if (valueString == "float") {
            return Type((TypeDeclaration *) &_float);
        } else if (valueString == "double") {
            return Type((TypeDeclaration *) &_double);
        } else if (valueString == "char") {
            return Type((TypeDeclaration *) &_char);
        } else if (valueString == "bool") {
            return Type((TypeDeclaration *) &_bool);
        } else {
            return UnexpectedTokenError(*token);
        }
    }

    // bool Type::linked() const {
    //     return _source->linked();
    // }

    std::string PrimitiveType::string() const {
        return std::format("Primitive(name:{},size:{},align:{})", _name, _size, _align);
    }

    std::string PrimitiveType::refString() const {
        return _name;
    }

    PrimitiveType::PrimitiveType(const std::string name, size_t size, size_t align) : 
        _name{name},
        _size{size},
        _align{align}
    {}

    PrimitiveType::PrimitiveType(const std::string name, size_t size) : 
        _name{name},
        _size{size},
        _align{size}
    {}

    // bool TypeDeclaration::linked() const {
    //     return true;
    // }

    // bool UnlinkedType::linked() const {
    //     return false;
    // }

    std::string Field::string() const {
        return std::format("Field(type:{},name:{})", type().string(), nameToken().source());
    }

    Field::Field(Type type, lexer::Token name) :
        _name{name},
        _type{type}
    {}

    const lexer::TokenValue Field::name() const {
        return nameToken().source();
    }

    const lexer::Token &Field::nameToken() const {
        return _name;
    }

    const Type Field::type() const {
        return _type;
    }

    Variable::Variable(Type type, lexer::Token name) : 
        Field(type, name)
    {}

    std::string Variable::string() const {
        return std::format("Variable(type:{},name:{})", type().string(), *nameToken().sourceString());
    }

    Parameter::Parameter(Type type, lexer::Token name) : 
        Field(type, name)
    {}

    std::string Parameter::string() const {
        return std::format("Parameter(type:{},name:{})", type().string(), *nameToken().sourceString());
    }

    Failable<Parameter *> Parameter::parse(Parser &parser) {
        logger::log("Parsing Parameter");
        Failable<Type> type = Type::parse(parser);
        if (!type.valid()) return static_cast<Failable<Parameter *>>(type);
        Failable<lexer::Token> name = parser.getToken(lexer::TokenType::IDENTIFIER);
        return new Parameter(*type, *name);
    }

    Function::Function(lexer::Token name, Type type, std::vector<Parameter *> &&parameters/*, std::vector<Statement *> &&statements*/) :
        _name{name},
        _type{type},
        _parameters{parameters}
    {}

    std::string Function::string() const {
        return std::format("Function(type:{},name:{},parameters:[{}])", type().string(), *nameToken().sourceString(), pvecToString(parameters()));
    }

    lexer::TokenValue Function::name() const {
        return _name.source();
    }

    const lexer::Token &Function::nameToken() const {
        return _name;
    }

    const Type &Function::type() const {
        return _type;
    }

    const std::vector<Parameter *> &Function::parameters() const {
        return _parameters;
    }

    Failable<Function *> Function::parse(Parser &parser, Type &type, lexer::Token &identifier) {
        logger::log("Parsing Function");
        std::vector<Parameter *> parameters = {};
        Failable<lexer::Token> openParen = parser.nextToken();
        if (!openParen.valid()) {
            return (Failable<Function *>) openParen;
        } else if (openParen->type() != lexer::TokenType::PUNC_LPAREN) {
            return UnexpectedTokenError(*openParen);
        }
        if (parser.peekTypeIs(lexer::TokenType::PUNC_RPAREN)) {
            parser.next();
        } else while (true) {
            Failable<Parameter *> param = Parameter::parse(parser);
            if (!param.valid()) return (Failable<Function *>) param;
            parameters.push_back(*param);
            Failable<lexer::Token> delimiterToken = parser.nextToken();
            if (!delimiterToken.valid()) {
                return (Failable<Function *>) delimiterToken;
            }
            if (delimiterToken->type() == lexer::TokenType::PUNC_RPAREN) break;
            else if (delimiterToken->type() != lexer::TokenType::PUNC_COMMA) {
                return UnexpectedTokenError(*delimiterToken);
            }
        }
        // REPLACE WITH ACTUAL BODY PARSING
        Failable<void> openbody = parser.skipToken(lexer::TokenType::PUNC_LCURLY);
        if (!openbody.valid()) return static_cast<Failable<Function *>>(openbody);
        Failable<void> closebody = parser.skipToken(lexer::TokenType::PUNC_RCURLY);
        if (!closebody.valid()) return static_cast<Failable<Function *>>(closebody);
        logger::log("Done parsing function");
        return new Function(identifier, type, std::move(parameters));
    }


    Failable<void> Parser::next() {
        if (storedToken.has_value()) {
            return static_cast<Failable<void>>(*std::exchange(storedToken, std::nullopt));
        }
        Failable<lexer::Token> token = _lexer.nextToken();
        return static_cast<Failable<void>>(token);
    }

    Failable<bool> Parser::skipIfToken(lexer::TokenType type) {
        Failable<lexer::Token> token = peekToken();
        if (!token.valid()) {
            return static_cast<Failable<bool>>(token);
        }
        if (token->type() == type) {
            next();
            return true;
        } else {
            return new UnexpectedTokenError(*token);
        }
    }

    Failable<void> Parser::skipToken(lexer::TokenType type) {
        Failable<lexer::Token> token = nextToken();
        if (!token.valid()) return static_cast<Failable<void>>(token);
        if (token->type() == type) {
            return Failable<void>();
        } else {
            return UnexpectedTokenError(*token);
        }
    }

    Failable<lexer::Token> Parser::nextToken() {
        if (storedToken.has_value()) {
            return *std::exchange(storedToken, std::nullopt);
        } else {
            return _lexer.nextToken();
        }
    }

    Failable<lexer::Token> Parser::getToken(lexer::TokenType type) {
        Failable<lexer::Token> token = nextToken();
        if (token.valid() && token->type() != type) return UnexpectedTokenError(*token);
        return token;
    }

    Failable<lexer::Token> Parser::peekToken() {
        if (storedToken.has_value()) {
            return *storedToken;
        } else {
            Failable<lexer::Token> token = _lexer.nextTokenOrEof();
            storedToken = std::make_optional(token);
            return token;
        }
    }

    Failable<lexer::TokenType> Parser::peekType() {
        Failable<lexer::Token> token = peekToken();
        if (!token.valid()) {
            return static_cast<Failable<lexer::TokenType>>(token);
        }
        return token->type();
    }

    bool Parser::peekTypeIs(lexer::TokenType type) {
        Failable<lexer::TokenType> otype = peekType();
        if (!otype.valid()) {
            return false;
        }
        return *otype == type;
    }

    bool Parser::peekTypeIs(bool (&classification)[lexer::countoftypes]) {
        Failable<lexer::Token> token = peekToken();
        if (!token.valid()) {
            return false;
        } else {
            return token->hasClassification(classification);
        }
    }

    Failable<void> Parser::parseFile() {
        logger::log("Parsing File");
        if (parsed) return Failable<void>();
        parsed = true;
        while (!peekTypeIs(lexer::TokenType::EOF_)) {
            logger::log("Not EOF!");
            Failable<Declaration *> declaration = Declaration::parse(*this);
            if (!declaration.valid()) {
                return static_cast<Failable<void>>(declaration);
            } else {
                logger::log("Parsed declaration ", (*declaration)->string());
            }
            fileDeclarations.push_back(*declaration);
        }
        logger::log("Returning!");
        return Failable<void>();
    }

    bool Parser::done() {
        return parsed;
    }

    std::string Parser::string() const {
        return pvecToString(fileDeclarations);
    }

    // UnlinkedType::UnlinkedType(std::vector<lexer::Token> &&tokens) :
    //     _nameTokens{std::move(tokens)}
    // {}

    // UnlinkedType::UnlinkedType(lexer::Token token) :
    //     _nameTokens{token}
    // {}
}