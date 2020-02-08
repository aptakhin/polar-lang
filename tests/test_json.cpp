#include <gtest/gtest.h>

#include "polar-lang/ast.h"

using namespace polar;

TEST(Json, RegexpRule) {
    auto term_node = std::make_unique<TermNode>(
            std::make_unique<StringTerm>("abc"));
    auto args = UNodeSeq{};
    args.push_back(std::move(term_node));
    auto rule = std::make_unique<RegexpRuleNode>(std::move(args));
    const auto res = rule->dump();

    ASSERT_EQ(res["args"][0]["value"], "abc");
//    std::cout << res.dump(4);
}