#pragma once

#include <sstream>

#include "polar-lang/ast.h"
#include "polar-lang/lexer.h"

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

    UNode make_term_node(int tok) const;

    UNode make_string_node(String str) const;

private:
    UNodeSeq flow_;

    Lexer lexer_;


};

} // namespace polar {

