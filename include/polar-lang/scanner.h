#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096

typedef struct _Scanner {
    /* Scanner state. */
    int cs;
    int act;
    int have;
    int curline;
    char *ts;
    char *te;
    char *p;
    char *pe;
    char *eof;
    FILE *file;
    int done;

    /* Token data */
    char *data;
    int len;
    int value;

    char buf[BUFSIZE];
} Scanner;

void scan_init(Scanner *s, FILE *file);

#define TK_NO_TOKEN (-1)
#define TK_ERR 128
#define TK_EOF 129
#define TK_Identifier 130
#define TK_Number 131
#define TK_String 132
#define TK_Event 133
#define TK_Endline 134

#define ret_tok( _tok ) { token = _tok; s->data = s->ts; }


int scan(Scanner *s);