#include <gtest/gtest.h>

#include "polar-lang/lexer.h"

using namespace polar;

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

TEST(Lexer, Lexems) {
    Lexer lexer;

    std::istringstream iss("abc 123 $ * #");
    lexer.load(iss);

    const auto items = std::vector<Item>{
        Item{"abc", TK_Identifier, 0},
        Item{"123", TK_Number, 4},
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
            if (item_counter >= items.size()) {
                throw std::runtime_error("Too few parse items!");
            }
            ASSERT_EQ(items[item_counter].type,
                      tok);
            ASSERT_EQ(items[item_counter].offset,
                      lexer.data - lexer.buf);
            ASSERT_EQ(items[item_counter].str,
                    String(lexer.data, lexer.data + lexer.len));
            item_counter += 1;
        }
    }
}