#include "test_ast.h"

namespace polar {

bool ensure_call(const char* value, const UNode& node, const char* func_name) {
    if (!node->is<CallFuncNode>()) {
        return false;
    }

    const auto& call_func = node->as<CallFuncNode>();
    if (!call_func->args()[0]->is<TermNode>()) {
        return false;
    }

    const auto& term = call_func->args()[0]->as<TermNode>()->term();
    if (!term->is<StringTerm>()) {
        return false;
    }

    const auto& func_name_term = term->as<StringTerm>();
    if (func_name_term->value() != func_name) {
        return false;
    }

    const auto& func_arg = call_func->args()[1]->as<TermNode>()->term()->as<StringTerm>();
    if (func_arg->value() != value) {
        return false;
    }

    return true;
}

bool ensure_call_context_get(const char* value, const UNode& node) {
    return ensure_call(value, node, "polar.get_context");
}

bool ensure_call_context_set(const char* value, const UNode& node) {
    return ensure_call(value, node, "polar.set_context");
}

bool ensure_call_event_get(const char* value, const UNode& node) {
    return ensure_call(value, node, "polar.get_event");
}

bool ensure_term_string(const char* value, const UNode& node) {
    if (!node->is<TermNode>()) {
        return false;
    }
    if (!node->as<TermNode>()->term()->is<StringTerm>()) {
        return false;
    }
    if (node->as<TermNode>()->term()->as<StringTerm>()->value() != value) {
        return false;
    }

    return true;
}

} // namespace polar {
