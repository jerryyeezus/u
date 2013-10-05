#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main ( int argc, const char *argv[] )
{
    char *b;
    char a[500];

    strcpy ( a, "kesha" );
    b = strtok ( a, "|" );
    printf ( "b = %s\n", b );
    b = strtok ( NULL, "|" );
    printf ( "b = %s\n", b );

    return 0;
}
