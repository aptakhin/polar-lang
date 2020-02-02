#include "polar-lang/parser.h"

namespace polar {

void ParserState::load(std::istream& in) {
    lexer_.load(in);

    while (true) {
        const auto tok = lexer_.next_lexeme();
        if (tok == TK_EOF) {
//            printf("parser: EOF\n");
            break;
        } else if (tok == TK_ERR) {
            throw std::runtime_error("Error in parsing (TODO: lines here)");
        } else {
//            printf("parser: %d \"", tok);
            fwrite(lexer_.data, 1, lexer_.len, stdout);
//            printf("\"\n" );

            if (tok == TK_Rule) {
                auto node = read_condition_rule();
                flow_.emplace_back(std::move(node));
            } else if (tok == TK_Response) {
                auto node = read_response();
                flow_.emplace_back(std::move(node));
            }
        }
    }
}

UNodeSeq ParserState::read_line() {
    UNodeSeq args;

    String resp_str_buf;

    enum {
        S_START,
        S_READ,
    };

    int state = S_START;

    while (true) {
        const auto tok = lexer_.next_lexeme();
        if (tok == TK_EOF || tok == TK_Endline) {
            break;
        }

        // Read lexems and pass nodes to arguments
        // Join string type tokens by ` ` (space)
        if (state == S_START) {
            if (is_term_tok(tok)) {
                if (tok == TK_String) {
                    resp_str_buf += lexer_.lexeme_str();
                    state = S_READ;
                } else {
                    args.push_back(make_term_node(tok));
                }
            }
        } else if (state == S_READ) {
            const auto term_tok = is_term_tok(tok);
            if (tok == TK_String) {
                resp_str_buf += " " + lexer_.lexeme_str();
            } else {
                // Push previous cached buf
                args.push_back(make_string_node(std::move(resp_str_buf)));
                state = S_START;

                if (is_term_tok(tok)) {
                    args.push_back(make_term_node(tok));
                }
            }
        } else {
            throw std::runtime_error("Unhandled state: "
                + std::to_string(state));
        }
    }

    if (state == S_READ) {
        // Finish it if have cache
        args.push_back(make_string_node(std::move(resp_str_buf)));
    }

    return args;
}

UNode ParserState::read_condition_rule() {
    UNodeSeq args = read_line();
    auto rule = std::make_unique<RegexpRuleNode>(std::move(args));
    return rule;
}

UNode ParserState::read_response() {
    UNodeSeq args = read_line();
    auto rule = std::make_unique<ResponseNode>(std::move(args));
    return rule;
}

bool ParserState::is_term_tok(int tok) {
    return tok == TK_String || tok == TK_Identifier || tok == TK_Kleine;
}

UNode ParserState::make_string_node(String str) const {
    auto term = std::make_unique<StringTerm>(std::move(str));
    return std::make_unique<TermNode>(std::move(term));
}

UNode ParserState::make_term_node(int tok) const {
    UTerm term;
    switch (tok) {
    case TK_String:
    case TK_Identifier:
        term = std::make_unique<StringTerm>(lexer_.lexeme_str());
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
