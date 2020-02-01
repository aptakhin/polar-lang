#include <gtest/gtest.h>

#include "polar-lang/lexer.h"

using namespace polar;

/// Testing item
struct Item {
    Item(String str, int type, int offset)
    :   str(std::move(str)),
        type(type),
        offset(offset) {
    }

    const String str;
    const int type;
    const int offset;
};

/// Helper for testing item
::testing::AssertionResult test_item(const Lexer& lexer,
        const std::vector<Item>& items, int tok, size_t item_counter) {
    if (item_counter >= items.size()) {
        throw std::runtime_error("Too few parse items!");
    }

    if (items[item_counter].type != tok) {
        return ::testing::AssertionFailure() << "Expected: "
            << items[item_counter].type << ". Got: " << tok
            << ". Item: " << item_counter;
    }

    const auto lexer_offset = lexer.data - lexer.buf;
    if (items[item_counter].offset != lexer.data - lexer.buf) {
        return ::testing::AssertionFailure() << "Expected: "
            << items[item_counter].type << ". Got: " << lexer_offset
            << ". Item: " << item_counter;
    }

    const auto lexer_value = String(lexer.data, lexer.data + lexer.len);
    if (items[item_counter].str != lexer_value) {
        return ::testing::AssertionFailure() << "Expected: `"
            << items[item_counter].str << "`. Got: `" << lexer_value << "`"
            << ". Item: " << item_counter;
    }

    return ::testing::AssertionSuccess() << "Item ok";
}

TEST(Lexer, Lexems) {
    Lexer lexer;

    std::istringstream iss("abc 123 $ * #");
    lexer.load(iss);

    const auto items = std::vector<Item>{
        Item{"abc 123 ", TK_String, 0},
        Item{"$", TK_Rule, 8},
        Item{"*", TK_Kleine, 10},
        Item{"#", TK_Response, 12},
    };
    size_t item_counter = 0;

    while (true) {
        const auto tok = lexer.next_lexeme();
        if (tok == TK_EOF) {
            if (item_counter != items.size()) {
                throw std::runtime_error("Not finished");
            }
            break;
        } else if (tok == TK_ERR) {
            throw std::runtime_error("Error in parsing");
        } else {
            EXPECT_TRUE(test_item(lexer, items, tok, item_counter));
            item_counter += 1;
        }
    }
}