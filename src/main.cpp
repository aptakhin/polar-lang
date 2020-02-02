#include <iostream>

#include "polar-lang/parser.h"

using namespace polar;

int main (int argc, char** argv)
{
    ParserState parser;
    parser.load(std::cin);

    std::cout << to_json(parser.flow()).dump() << std::endl;
    return 0;
}
