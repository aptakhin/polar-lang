#pragma once

#include <sstream>

#include "polar-lang/ast.h"
#include "polar-lang/lexer.h"

namespace polar {

class ParserState {
public:
    ParserState() = default;

    void load(std::istream& in);

protected:
    UNodeSeq flow_;

    Lexer lexer_;
};

} // namespace polar {

