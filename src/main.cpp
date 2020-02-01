#include <iostream>

#include "polar-lang/ast.h"
#include "polar-lang/scanner.h"

int main (int argc, char** argv)
{
    Scanner ss;
    int tok;

    scan_init(&ss, stdin);

    while ( 1 ) {
        tok = scan (&ss);
        if ( tok == TK_EOF ) {
            printf ("parser: EOF\n");
            break;
        }
        else if ( tok == TK_ERR ) {
            printf ("parser: ERR\n");
            break;
        }
        else {
            printf ("parser: %d \"", tok);
            fwrite ( ss.data, 1, ss.len, stdout );
            printf ("\"\n" );
        }
    }

    return 0;
}
