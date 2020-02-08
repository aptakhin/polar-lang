#include <gtest/gtest.h>

#include "polar-lang/ast.h"
#include "polar-lang/parser.h"

#include "test_ast.h"

using namespace polar;


TEST(Parser, RegexpRule) {
    ParserState parser;

    std::istringstream iss("$ abc\n# def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 2);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);
    const auto rule = flow[0]->as<RuleNode>();
    ASSERT_EQ(rule->args().size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);

    ASSERT_EQ(rule->args()[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = rule->args()[0]->as<RegexpRuleNode>();
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
    ASSERT_EQ(flow[0]->type(), ENode::RULE);
    const auto rule = flow[0]->as<RuleNode>();
    ASSERT_EQ(rule->args().size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);

    ASSERT_EQ(rule->args()[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = rule->args()[0]->as<RegexpRuleNode>();
    ASSERT_EQ(regexp->args().size(), 3);

    ASSERT_TRUE(regexp->args()[0]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[0]->as<TermNode>()->term()->is<KleineTerm>());

    ASSERT_TRUE(regexp->args()[1]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[1]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(regexp->args()[1]->as<TermNode>()->term()->as<StringTerm>()->value(), "abc");

    ASSERT_TRUE(regexp->args()[2]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[2]->as<TermNode>()->term()->is<KleineTerm>());
}

TEST(Parser, RegexpRuleMergeText) {
    ParserState parser;

    std::istringstream iss("$ abc def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);
    const auto rule = flow[0]->as<RuleNode>();
    ASSERT_EQ(rule->args().size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);

    ASSERT_EQ(rule->args()[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = rule->args()[0]->as<RegexpRuleNode>();
    ASSERT_EQ(regexp->args().size(), 1);

    ASSERT_TRUE(regexp->args()[0]->is<TermNode>());
    ASSERT_TRUE(regexp->args()[0]->as<TermNode>()->term()->is<StringTerm>());
    ASSERT_EQ(regexp->args()[0]->as<TermNode>()->term()->as<StringTerm>()->value(), "abc def");
}

TEST(Parser, IfEvent) {
    ParserState parser;

    std::istringstream iss("$ if event abc = def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);
    const auto rule_ = flow[0]->as<RuleNode>();
    ASSERT_EQ(rule_->args().size(), 1);

    ASSERT_TRUE(rule_->args()[0]->is<CallFuncNode>());
    const auto if_cond = rule_->args()[0]->as<CallFuncNode>();
    ASSERT_TRUE(ensure_term_string("=", if_cond->args()[0]));
    ASSERT_TRUE(ensure_call_event_get("abc", if_cond->args()[1]));
    ASSERT_TRUE(ensure_term_string("def", if_cond->args()[2]));
}


TEST(Parser, IfContext) {
    ParserState parser;

    std::istringstream iss("$ if get abc = def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RULE);
    const auto rule_ = flow[0]->as<RuleNode>();
    ASSERT_EQ(rule_->args().size(), 1);

    ASSERT_TRUE(rule_->args()[0]->is<CallFuncNode>());
    const auto if_cond = rule_->args()[0]->as<CallFuncNode>();
    ASSERT_TRUE(ensure_term_string("=", if_cond->args()[0]));
    ASSERT_TRUE(ensure_call_context_get("abc", if_cond->args()[1]));
    ASSERT_TRUE(ensure_term_string("def", if_cond->args()[2]));

    std::cout << to_json(parser).dump(4) << std::endl;
}

TEST(Parser, ContextSet) {
    ParserState parser;

    std::istringstream iss("# set abc def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 1);
    ASSERT_EQ(flow[0]->type(), ENode::RESPONSE);
    const auto response = flow[0]->as<ResponseNode>();
    ASSERT_EQ(response->args().size(), 1);

    ASSERT_EQ(response->args()[0]->type(), ENode::CALL_FUNC);
    ASSERT_TRUE(ensure_call_context_set("abc", response->args()[0]));

    std::cout << to_json(parser).dump(4) << std::endl;
}


TEST(Parser, StarFail) {
    ParserState parser;

    std::istringstream iss("$ *\n"
                           "# Don't underestimate\n"
                           "# Pity not to understand\n\n");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow.size(), 3);
    std::cout << to_json(parser).dump(4) << std::endl;
    int p = 0;
}
