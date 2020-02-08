#pragma once

#include <list>
#include "polar-lang/common.h"
#include "polar-lang/lexer.h"

namespace polar {

struct Lexeme {
    int tok;
    String text;
};

class LexerBuffer {
public:
    LexerBuffer() = default;

    void load(Lexer& lexer);

    Lexeme pull();

    void unget(Lexeme lexeme);

private:
    Lexer* lexer_{};

    std::list<Lexeme> buf_;
};

} // namespace polar {
