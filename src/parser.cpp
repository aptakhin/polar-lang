#include "polar-lang/parser.h"



namespace polar {

void ParserState::load(std::istream& in) {
    lexer_.load(in);

    while (true) {
        const auto tok = lexer_.next_lexeme();
        if (tok == TK_EOF) {
            printf("parser: EOF\n");
            break;
        } else if (tok == TK_ERR) {
            printf("parser: ERR\n");
            break;
        } else {
            printf("parser: %d \"", tok);
            fwrite(lexer_.data, 1, lexer_.len, stdout);
            printf("\"\n" );
        }
    }
}

} // namespace polar {
