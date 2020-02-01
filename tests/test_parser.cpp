#include <gtest/gtest.h>

#include "polar-lang/parser.h"

using namespace polar;

TEST(Parser, RegexpRule) {
    ParserState parser;

    std::istringstream iss("$ abc\n# def");
    parser.load(iss);

    const auto& flow = parser.flow();

    ASSERT_EQ(flow[0]->type(), ENode::REGEXP_RULE);
    const auto regexp = flow[0]->as<RegexpRuleNode>();
//    ASSERT_EQ(regexp->, ENode::REGEXP_RULE);
//    ASSERT_EQ(flow[1]->type(), ENode::RESPONSE);
}