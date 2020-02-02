#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <istream>

#include "polar-lang/common.h"

#define BUFSIZE 4096

namespace polar {

struct Lexer {
    /* Lexer state. */
    int cs;
    int act;
    int have;
    int curline;
    unsigned char *ts;
    unsigned char *te;
    unsigned char *p;
    unsigned char *pe;
    unsigned char *eof;
    std::istream *file;
    int done;

    /* Token data */
    unsigned char *data;
    int len;
    int value;

    unsigned char buf[BUFSIZE];

public:
    Lexer();

    void load(std::istream &file);

    int next_lexeme();

    String lexeme_str() const;
};


#define TK_NO_TOKEN (-1)
#define TK_ERR 128
#define TK_EOF 129
#define TK_Identifier 130
#define TK_Number 131
#define TK_String 132
#define TK_Event 133
#define TK_Endline 134
#define TK_Rule 135
#define TK_Response 136
#define TK_Kleine 137

#define ret_tok(_tok) { token = _tok; s->data = s->ts; }

} // namespace polar {