#include "polar-lang/parser.h"

namespace polar {

void ParserState::load(std::istream& in) {
    Lexer lexer__;
    lexer__.load(in);
    lexer_buf_.load(lexer__);

    while (true) {
        const auto lexeme = lexer_buf_.pull();
        if (lexeme.tok == TK_EOF) {
//            printf("parser: EOF\n");
            break;
        } else if (lexeme.tok == TK_ERR) {
            throw std::runtime_error("Error in parsing (TODO: lines here)");
        } else {
            if (lexeme.tok == TK_Rule) {
                const auto check_next = lexer_buf_.pull();

                UNode rule_arg;
                if (check_next.tok == TK_If) {
                    rule_arg = read_if();
                } else {
                    lexer_buf_.unget(check_next);
                    rule_arg = read_condition_rule();
                }

                UNodeSeq args;
                args.push_back(std::move(rule_arg));

                auto rule = std::make_unique<RuleNode>(std::move(args));
                flow_.emplace_back(std::move(rule));

            } else if (lexeme.tok == TK_Response) {
                const auto check_next = lexer_buf_.pull();
                if (check_next.tok == TK_Set) {
                    auto node = read_set();
                    UNodeSeq args;
                    args.push_back(std::move(node));
                    auto resp = std::make_unique<ResponseNode>(std::move(args));
                    flow_.emplace_back(std::move(resp));
                } else {
                    lexer_buf_.unget(check_next);
                    auto node = read_response();
                    flow_.emplace_back(std::move(node));
                }
            } else if (lexeme.tok == TK_Endline) {
                continue;
            } else {
                throw std::runtime_error("Invalid parser state at " +
                std::to_string(lexeme.tok));
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
        const auto lexeme = lexer_buf_.pull();
        if (lexeme.tok == TK_EOF || lexeme.tok == TK_Endline) {
            break;
        }

        // Read lexems and pass nodes to arguments
        // Join string type tokens by ` ` (space)
        if (state == S_START) {
            if (is_term_tok(lexeme.tok)) {
                if (lexeme.tok == TK_String) {
                    resp_str_buf += lexeme.text;
                    state = S_READ;
                } else {
                    args.push_back(make_term_node(lexeme));
                }
            }
        } else if (state == S_READ) {
            const auto term_tok = is_term_tok(lexeme.tok);
            if (lexeme.tok == TK_String) {
                resp_str_buf += " " + lexeme.text;
            } else {
                // Push previous cached buf
                args.push_back(make_string_node(std::move(resp_str_buf)));
                state = S_START;

                if (is_term_tok(lexeme.tok)) {
                    args.push_back(make_term_node(lexeme));
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

UNode ParserState::make_term_node(const Lexeme& lexeme) const {
    UTerm term;
    switch (lexeme.tok) {
    case TK_String:
    case TK_Identifier:
        term = std::make_unique<StringTerm>(lexeme.text);
        break;
    case TK_Kleine:
        term = std::make_unique<KleineTerm>();
        break;
    default:
        break;
    }

    if (!term) {
        throw std::runtime_error("Unhandled term: "
            + std::to_string(lexeme.tok));
    }

    return std::make_unique<TermNode>(std::move(term));
}


UNode ParserState::read_if_impl() {
    return UNode{};
}

UNode ParserState::read_if() {
    UNodeSeq args;

    while (true) {
        const auto lexeme = lexer_buf_.pull();
        if (lexeme.tok == TK_EOF || lexeme.tok == TK_Endline) {
            break;
        }

        if (lexeme.tok == TK_Event) {
            const auto& event_kw = lexeme;

            const auto event_uuid = lexer_buf_.pull();
            if (event_uuid.tok != TK_String) {
                throw std::runtime_error("Expected string: "
                                         + std::to_string(event_uuid.tok));
            }

            const auto op = lexer_buf_.pull();
            if (op.tok != TK_Eq && op.tok != TK_Neq) {
                throw std::runtime_error("Expected op: "
                                         + std::to_string(op.tok));
            }

            const auto value = lexer_buf_.pull();
            if (value.tok != TK_String) {
                throw std::runtime_error("Expected string: "
                                         + std::to_string(value.tok));
            }

            UNode init[] = {
                make_string_node(op.text),
                make_event_get(event_uuid.text),
                make_string_node(value.text),
            };
            UNodeSeq call_args{std::make_move_iterator(std::begin(init)),
                               std::make_move_iterator(std::end(init))};

            auto cmd = std::make_unique<CallFuncNode>(
                std::move(call_args)
            );
            args.push_back(std::move(cmd));
        } else if (lexeme.tok == TK_Get) {
            const auto& event_kw = lexeme;

            const auto event_uuid = lexer_buf_.pull();
            if (event_uuid.tok != TK_String) {
                throw std::runtime_error("Expected string: "
                                         + std::to_string(event_uuid.tok));
            }

            const auto op = lexer_buf_.pull();
            if (op.tok != TK_Eq && op.tok != TK_Neq) {
                throw std::runtime_error("Expected op: "
                                         + std::to_string(op.tok));
            }

            const auto value = lexer_buf_.pull();
            if (value.tok != TK_String) {
                throw std::runtime_error("Expected string: "
                                         + std::to_string(value.tok));
            }

            UNode init[] = {
                    make_string_node(op.text),
                    make_context_get(event_uuid.text),
                    make_string_node(value.text),
            };
            UNodeSeq call_args{std::make_move_iterator(std::begin(init)),
                               std::make_move_iterator(std::end(init))};

            auto cmd = std::make_unique<CallFuncNode>(
                    std::move(call_args)
            );
            args.push_back(std::move(cmd));
        } else {
            throw std::runtime_error("Unhandled if state: "
                                     + std::to_string(lexeme.tok));
        }
    }

//    auto rule = std::make_unique<IfNode>(std::move(args));

    UNode v = std::move(args[0]);
    return v;
}

UNode ParserState::make_event_get(String event) {
    UNode init[] = {
        make_string_node("polar.get_event"),
        make_string_node(std::move(event)),
    };
    UNodeSeq call_args{std::make_move_iterator(std::begin(init)),
                       std::make_move_iterator(std::end(init))};

    auto cmd = std::make_unique<CallFuncNode>(
         std::move(call_args)
    );
    return cmd;
}

UNode ParserState::make_context_get(String context) {
    UNode init[] = {
        make_string_node("polar.get_context"),
        make_string_node(std::move(context)),
    };
    UNodeSeq call_args{std::make_move_iterator(std::begin(init)),
                       std::make_move_iterator(std::end(init))};

    auto cmd = std::make_unique<CallFuncNode>(
            std::move(call_args)
    );
    return cmd;
}

UNode ParserState::read_set() {
    UNodeSeq args;

    const auto context_varname = lexer_buf_.pull();
    if (context_varname.tok != TK_String) {
        throw std::runtime_error("Expected string: "
                                 + std::to_string(context_varname.tok));
    }

    const auto context_value = lexer_buf_.pull();
    if (context_value.tok != TK_String) {
        throw std::runtime_error("Expected string: "
                                 + std::to_string(context_varname.tok));
    }

    UNode init[] = {
        make_string_node("polar.set_context"),
        make_string_node(context_varname.text),
        make_string_node(context_value.text),
    };
    UNodeSeq call_args{std::make_move_iterator(std::begin(init)),
                       std::make_move_iterator(std::end(init))};

    auto cmd = std::make_unique<CallFuncNode>(
         std::move(call_args)
    );
    return cmd;
}

json to_json(const ParserState& state) {
    return json({
        {"node", "flow"},
        {"flow", to_json(state.flow())},
    });
}

} // namespace polar {
