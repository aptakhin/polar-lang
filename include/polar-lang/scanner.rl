#include "polar-lang/scanner.h"

%%{
    machine Scanner;
    write data;
}%%

void scan_init( Scanner *s, FILE *file )
{
    memset (s, '\0', sizeof(Scanner));
    s->curline = 1;
    s->file = file;
    s->eof = 0;
    %% write init;
}

int scan( Scanner *s )
{
	int token = TK_NO_TOKEN;
	int space, readlen;

	while ( 1 ) {
		if ( s->p == s->pe ) {
			printf("scanner: need more data\n");

			if ( s->ts == 0 )
				s->have = 0;
			else {
				/* There is data that needs to be shifted over. */
				printf("scanner: buffer broken mid token\n");
				s->have = s->pe - s->ts;
				memmove( s->buf, s->ts, s->have );
				s->te -= (s->ts-s->buf);
				s->ts = s->buf;
			}

			s->p = s->buf + s->have;
			space = BUFSIZE - s->have;

			if ( space == 0 ) {
				/* We filled up the buffer trying to scan a token. */
				printf("scanner: out of buffer space\n");
				return TK_ERR;
			}

			if ( s->done ) {
				printf("scanner: end of file\n");
				s->p[0] = 0;
				readlen = 1;
			}
			else {
				readlen = fread( s->p, 1, space, s->file );
				if ( readlen < space )
					s->done = 1;
			}

			s->pe = s->p + readlen;
		}

		%%{
			machine Scanner;
			access s->;
			variable p s->p;
			variable pe s->pe;
			variable eof s->eof;

			main := |*

            "EVENT" =>
                { ret_tok( TK_Event ); fbreak; };

			# Identifiers
			( [a-zA-Z_] [a-zA-Z0-9_]* ) =>
				{ ret_tok( TK_Identifier ); fbreak; };

			[\n] => { ret_tok( TK_Endline ); fbreak; };

			# Whitespace
			[ \t];

			'"' ( [^\\"] | '\\' any ) * '"' =>
				{ ret_tok( TK_String ); fbreak; };

			# Number
			digit+ =>
				{ ret_tok( TK_Number ); fbreak; };

			# EOF
			0 =>
				{ ret_tok( TK_EOF ); fbreak; };

			# Anything else
			any =>
				{ ret_tok( *s->p ); fbreak; };

			*|;

			write exec;
		}%%

		if ( s->cs == Scanner_error )
			return TK_ERR;

		if ( token != TK_NO_TOKEN ) {
			s->len = s->p - s->data;
			return token;
		}
	}
}

