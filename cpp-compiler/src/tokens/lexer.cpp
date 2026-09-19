#include "lexer.hpp"
#include "utils/compiler-utils.hpp"
#include <string>
#include <format>

namespace compiler::lexer {
    std::vector<std::string> sources = {};
    std::unordered_map<std::string, size_t> map = {};

    TokenValue getSource(std::string &source) {
        if (map.contains(source)) {
            return map[source];
        } else {
            TokenValue id = map[source] = sources.size();
            sources.push_back(source);
            return id;
        }
    }

    Token::Token(TokenType type, Location location, std::string &source) :
        _type{type},
        _location{location},
        _source{getSource(source)}
    {}

    TokenValue Token::source() {
        return _source;
    }

    const Location &Token::location() {
        return _location;
    }

    std::string Token::string() {
        return std::format("{{Token (at {}:{} in {}) type={} source=\"{}\"}}", _location.line, _location.column, _location.file->relative_path().string(), static_cast<int>(_type), *sourceString());
    }

    std::string const *const Token::sourceString() {
        return &sources[_source];
    }

    TokenType Token::type() {
        return _type;
    }

    bool Token::hasClassification(bool (&classification)[countoftypes]) {
        return classification[(size_t) _type];
    }

    Lexer::Lexer(options::SourceFile &&source) :
        _source{std::move(source)}
    {}

    bool Lexer::done() {
        return _done;
    }

    bool Lexer::incrementLine() {
        if (std::getline(_source.stream, currentLine)) {
            column = 0;
            line++;
            getCurrentChar();
            return true;
        } else {
            _done = true;
            return false;
        }
    }

    const std::filesystem::path &Lexer::file() {
        return _source.path;
    }

    Location Lexer::getTokenLocation(size_t offset) {
        return Location(line, column - offset + 1, &_source.path);
    }

    char Lexer::getNextChar() {
        column++;
        return getCurrentChar();
    }

    char Lexer::getCurrentChar() {
        if (column >= currentLine.length()) return current = '\n';
        else return current = currentLine[column];
    }

    constexpr bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    constexpr bool isPunctuation(char c) {
        return c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == '[' || c == ']'
            || c == ':' || c == ',';
    }

    constexpr bool isOperator(char c) {
        return c == '!' || c == '%' || c == '^' || c == '&' || c == '*' || c == '-' || c == '='
            || c == '+' || c == '|' || c == '<' || c == '>' || c == '?' || c == '~' || c == '.';
    }

    constexpr bool isStringDelimiter(char c) {
        return c == '`' || c == '\'' || c == '"';
    }

    constexpr bool isNumeric(char c) {
        return c >= '0' && c <= '9';
    }

    constexpr bool isAlpha(char c) {
        return ((c | ' ') >= 'a' && (c | ' ') <= 'z') || c == '#' || c == '$' || c == '_';
    }

    constexpr bool isAlphaNumeric(char c) {
        return isAlpha(c) || isNumeric(c);
    }

    Failable<Token> Lexer::constructToken(TokenType type, size_t length, std::string value) {
        return Failable(Token(type, getTokenLocation(length), value));
    }

    Failable<Token> Lexer::constructToken(TokenType type, size_t length) {
        return constructToken(type, length, currentLine.substr(column - length, length));
    }

    Failable<Token> Lexer::nextTokenOrEof() {
        if (done()) {
            return Failable<Token>(new CompileError(getTokenLocation(), "Tried to get token from already completed lexer."));
        }

        while (column < currentLine.size() && isWhitespace(current)) {
            getNextChar();
        }

        if (column >= currentLine.size()) {
            if (!incrementLine()) {
                logger::log("Done tokenizing file ", file().relative_path().string());
                std::string s = "";
                return Failable(Token(TokenType::EOF_, getTokenLocation(), s));
            } else [[likely]] {
                return nextToken();
            }
        }
        
        getCurrentChar();

        if (isPunctuation(current)) {
            return parsePunctuation();
        } else if (
                isNumeric(current)
                 || (current == '.' && currentLine.length() > column + 1
                 && isNumeric(currentLine[column + 1]))
            ) 
        {
            return parseNumeric();
        } else if (isOperator(current)) {
            return parseOperator();
        // } else if (isStringDelimiter(current)) {
        //     return parseString();
        } else if (isAlpha(current)) {
            return parseIdentifier();
        } else {
            _done = true;
            return Failable<Token>(new CompileError(getTokenLocation(), std::format("Unexpected start of token: '{}'", current)));
        }
    }

    Failable<Token> Lexer::nextToken() {
        Failable<Token> token = nextTokenOrEof();
        if (token.valid() && token.value().type() == TokenType::EOF_) return Failable<Token>(new CompileError(getTokenLocation(), "Unexpected end of file."));
        else [[likely]] return token;
    }

    Failable<Token> Lexer::parsePunctuation() {
        TokenType type;
        switch(currentLine[column]) {
            case '(':
                type = TokenType::PUNC_LPAREN;
                break;
            case ')':
                type = TokenType::PUNC_RPAREN;
                break;
            case '{':
                type = TokenType::PUNC_LCURLY;
                break;
            case '}':
                type = TokenType::PUNC_LCURLY;
                break;
            case ';':
                type = TokenType::PUNC_SEMICOLON;
                break;
            case ',':
                type = TokenType::PUNC_COMMA;
                break;
            default:
                return Failable<Token>(new CompileError(getTokenLocation(), std::format("Unexpected token '{}'", current)));
        }
        getNextChar();
        return constructToken(type, 1);
    }

    Failable<Token> Lexer::parseNumeric() {
        bool usedDecimal = false;
        size_t numberStart = column;
        while (isNumeric(current) || (current == '.' && !usedDecimal) || current == '_') {
            if (current == '.') usedDecimal = true;
            getNextChar();
        }
        return constructToken(usedDecimal ? TokenType::LIT_FLOAT : TokenType::LIT_INT, column - numberStart);
    }

    Failable<Token> Lexer::parseIdentifier() {
        size_t idStart = column;
        while (isAlphaNumeric(current)) {
            getNextChar();
        }
        std::string identifier = currentLine.substr(idStart, column - idStart);
        if (identifier == "true" || identifier == "false") {
            return constructToken(TokenType::LIT_BOOL, identifier.length(), identifier);
        } else if ( identifier == "int" || identifier == "float" || identifier == "bool"
                 || identifier == "char" || identifier == "long" || identifier == "double") {
            // ignoring other types for now, just going for basic functionality
            return constructToken(TokenType::KW_PRIMITIVE, identifier.length(), identifier);
        } else [[likely]] {
            return constructToken(TokenType::IDENTIFIER, identifier.length(), identifier);
        }
    }

    Failable<Token> Lexer::parseOperator() {
        if (current == '=') {
            getNextChar();
            return constructToken(TokenType::OP_ASSIGN, 1);
        } else {
            return Failable<Token>(new CompileError(getTokenLocation(), std::format("Unexpected token '{}'", current)));
        }
    }
}