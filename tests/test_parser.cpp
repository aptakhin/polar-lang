#include <gtest/gtest.h>

#include "polar-lang/ast.h"
#include "polar-lang/parser.h"

using namespace polar;

TEST(Parser, RegexpRule) {
    ParserState parser;

    std::istringstream iss("$ abc\n# def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 2);
    ASSERT_EQ(flow[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = flow[0]->as<RegexpRuleNode>();
    ASSERT_EQ(regexp->args().size(), 1);
    ASSERT_TRUE(regexp->args()[0]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[0]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(regexp->args()[0]->as<TermNode>()->term()->as<StringTerm>()->value(), "abc");

    ASSERT_EQ(flow[1]->type(), ENode::RESPONSE);
    const auto response = flow[1]->as<ResponseNode>();
    ASSERT_EQ(response->args().size(), 1);
    ASSERT_TRUE(response->args()[0]->is<TermNode>());
    ASSERT_TRUE(response->args()[0]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(response->args()[0]->as<TermNode>()->term()->as<StringTerm>()->value(), "def");
}

TEST(Parser, RegexpRuleKleine) {
    ParserState parser;

    std::istringstream iss("$ * abc *");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = flow[0]->as<RegexpRuleNode>();
    ASSERT_EQ(regexp->args().size(), 3);

    ASSERT_TRUE(regexp->args()[0]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[0]->as<TermNode>()->term()->is<KleineTerm>());

    ASSERT_TRUE(regexp->args()[1]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[1]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(regexp->args()[1]->as<TermNode>()->term()->as<StringTerm>()->value(), "abc");

    ASSERT_TRUE(regexp->args()[2]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[2]->as<TermNode>()->term()->is<KleineTerm>());
}

TEST(Parser, RegexpRuleKleineMergeText) {
    ParserState parser;

    std::istringstream iss("$ abc def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = flow[0]->as<RegexpRuleNode>();
    ASSERT_EQ(regexp->args().size(), 1);

    ASSERT_TRUE(regexp->args()[0]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[0]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(regexp->args()[0]->as<TermNode>()->term()->as<StringTerm>()->value(), "abc def");
}
