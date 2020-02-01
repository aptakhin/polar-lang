#include "polar-lang/lexer.h"

namespace polar {

%%{
    machine Lexer;
    write data;
}%%

Lexer::Lexer() {
    Lexer* s = this;
    memset (s, '\0', sizeof(Lexer));
    s->curline = 1;
    s->eof = 0;
    %% write init;
}

void Lexer::load(std::istream& f)
{
    file = &f;
}

int Lexer::next_lexeme()
{
    Lexer* s = this;
	int token = TK_NO_TOKEN;
	int space, readlen;

	while (1) {
		if (s->p == s->pe) {
			// printf("scanner: need more data\n");

			if (s->ts == 0)
				s->have = 0;
			else {
				/* There is data that needs to be shifted over. */
				// printf("scanner: buffer broken mid token\n");
				s->have = s->pe - s->ts;
				memmove(s->buf, s->ts, s->have);
				s->te -= (s->ts-s->buf);
				s->ts = s->buf;
			}

			s->p = s->buf + s->have;
			space = BUFSIZE - s->have;

			if (space == 0) {
				/* We filled up the buffer trying to scan a token. */
				// printf("scanner: out of buffer space\n");
				return TK_ERR;
			}

			if (s->done) {
				// printf("scanner: end of file\n");
				s->p[0] = 0;
				readlen = 1;
			}
			else {
				s->file->read(s->p, space);
				readlen = s->file->gcount();
				if (readlen < space) {
					s->done = 1;
			    }
			}

			s->pe = s->p + readlen;
		}

		%%{
			machine Lexer;
			access s->;
			variable p s->p;
			variable pe s->pe;
			variable eof s->eof;

			main := |*

            "$" =>
                { ret_tok(TK_Rule); fbreak; };

            "#" =>
                { ret_tok(TK_Response); fbreak; };

            "*" =>
                { ret_tok(TK_Kleine); fbreak; };

            "EVENT" =>
                { ret_tok(TK_Event); fbreak; };

			[\n] => { ret_tok(TK_Endline); fbreak; };

			[ \t]+;

			# Anything else
			[a-zA-Z0-9 \t.,!?]+ =>
				{ ret_tok(TK_String); fbreak; };

			# EOF
			0 =>
				{ ret_tok(TK_EOF); fbreak; };

			*|;

			write exec;
		}%%

		if (s->cs == Lexer_error) {
			return TK_ERR;
		}

		if (token != TK_NO_TOKEN) {
			s->len = s->p - s->data;
			return token;
		}
	}
}

} // namespace polar {