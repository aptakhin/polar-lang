#pragma once

#include "polar-lang/common.h"
#include "polar-lang/ast.h"

namespace polar {

bool ensure_call_context_get(const char* value, const UNode& node);
bool ensure_call_context_set(const char* value, const UNode& node);

bool ensure_call_event_get(const char* value, const UNode& node);
bool ensure_term_string(const char* value, const UNode& node);

} // namespace polar {
