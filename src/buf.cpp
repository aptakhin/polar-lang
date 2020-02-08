#include "polar-lang/buf.h"

namespace polar {

void LexerBuffer::load(Lexer& lexer) {
    lexer_ = &lexer;
}

Lexeme LexerBuffer::pull() {
    if (!lexer_) {
        throw std::runtime_error("Pull usage without loaded lexer!");
    }

    auto lexeme = Lexeme{};
    if (buf_.empty()) {
        int tok = lexer_->next_lexeme();
        lexeme.tok = tok;
        lexeme.text = lexer_->lexeme_str();
    } else {
        lexeme = std::move(buf_.front());
        buf_.pop_front();
    }

    return std::move(lexeme);
}

void LexerBuffer::unget(Lexeme lexeme) {
    buf_.push_front(std::move(lexeme));
}

} // namespace polar {