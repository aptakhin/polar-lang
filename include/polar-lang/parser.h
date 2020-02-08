#pragma once

#include <sstream>

#include "polar-lang/ast.h"
#include "polar-lang/lexer.h"
#include "polar-lang/buf.h"

namespace polar {

class ParserState {
public:
    ParserState() = default;

    void load(std::istream& in);

    const UNodeSeq& flow() { return flow_; }

private:

    UNode read_condition_rule();
    UNode read_response();

    UNodeSeq read_line();

    static bool is_term_tok(int tok);

    UNode make_term_node(const Lexeme& lexeme) const;

    UNode make_string_node(String str) const;

private:
    UNodeSeq flow_;

    LexerBuffer lexer_buf_;

    UNode read_if();

    UNode read_if_impl();

    UNode make_event_get(String event);

    UNode make_context_get(String context);

    UNode read_set();
};

} // namespace polar {

