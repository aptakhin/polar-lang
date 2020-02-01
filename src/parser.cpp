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

            if (tok == TK_Rule) {
                auto node = read_condition_rule();
                flow_.emplace_back(std::move(node));
            }
        }
    }

    int p = 0;
}

UNode ParserState::read_condition_rule() {
    UNodeSeq args;

    while (true) {
        const auto tok = lexer_.next_lexeme();
        if (tok == TK_EOF || tok == TK_Endline) {
            break;
        }

        if (is_term_tok(tok)) {
            auto term_node = make_term_node(tok);
            args.push_back(std::move(term_node));
        }
    }

    auto rule = std::make_unique<RegexpRuleNode>(std::move(args));
    return rule;
}

bool ParserState::is_term_tok(int tok) {
    return tok == TK_String || tok == TK_Identifier || tok == TK_Kleine;
}

UNode ParserState::make_term_node(int tok) const {
    UTerm term;
    switch (tok) {
    case TK_String:
    case TK_Identifier:
        term = std::make_unique<StringTerm>(
                String(lexer_.data, lexer_.data + lexer_.len));
        break;
    case TK_Kleine:
        term = std::make_unique<KleineTerm>();
        break;
    default:
        break;
    }

    if (!term) {
        throw std::runtime_error("Unhandled term: " + std::to_string(tok));
    }

    return std::make_unique<TermNode>(std::move(term));
}

} // namespace polar {
