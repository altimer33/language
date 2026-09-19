#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include "utils/compiler-utils.hpp"

namespace compiler::lexer {
    typedef size_t TokenValue;
    
    enum class TokenType : char {
        EOF_,
        IDENTIFIER,
        LIT_INT,
        LIT_FLOAT,
        LIT_CHAR,
        LIT_BOOL,
        PUNC_LPAREN,
        PUNC_RPAREN,
        PUNC_LBRACKET,
        PUNC_RBRACKET,
        PUNC_LCURLY,
        PUNC_RCURLY,
        PUNC_SEMICOLON,
        PUNC_COMMA,
        KW_PRIMITIVE, // Will be temporary, included with IDENTIFER once standard library starts and annotations work.
        OP_ASSIGN,
        // Many more to come, developing small parts at a time.
    };

    inline constexpr size_t countoftypes = 14;

    class Token {
    private:
        TokenType _type;
        Location _location;
        TokenValue _source;

    public:
        Token(TokenType type, Location location, std::string &source);

        TokenValue source();
        std::string const *const sourceString();
        TokenType type();
        const Location &location();

        std::string string();

        bool hasClassification(bool (&classification)[countoftypes]);
    };

    class Lexer {
    private:
        options::SourceFile _source;
        int line = 0;
        int column = 0;
        bool _done = false;
        std::string currentLine;
        char current;

        Failable<Token> parsePunctuation();
        Failable<Token> parseOperator();
        Failable<Token> parseIdentifier();
        Failable<Token> parseNumeric();
        // Failable<Token> parseString();
        // Failable<Token> parseAnnotation();

        char getCurrentChar();
        char getNextChar();
        bool incrementLine();

        Failable<Token> constructToken(TokenType tokenType, size_t length);
        Failable<Token> constructToken(TokenType tokenType, size_t length, std::string value);
        Location getTokenLocation(size_t offset = 0);

    public:
        Lexer(options::SourceFile &&source);

        bool done();
        const std::filesystem::path &file();

        Failable<Token> nextToken();
        Failable<Token> nextTokenOrEof();
    };

}