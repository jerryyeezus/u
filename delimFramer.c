#include "musicProtocol.h"

static const char DELIMITER = '\n';

int GetNextMsg ( FILE *in, uint8_t *buf, size_t bufSize )
{
    int count = 0;
    int nextChar;
    while ( count < bufSize )
    {
        nextChar = getc ( in );
        if ( nextChar == EOF )
        {
            if ( count > 0 )
                perror ( "GetNextMsg() stream ended prematurely" );
            else
                return -1;
        }
        if ( nextChar == DELIMITER )
            break;
        buf[count++] = nextChar;
    }
    if ( nextChar != DELIMITER )  		//Out of space: count==bufSize
    {
        return -count;
    }
    else  				//Found delimiter
    {
        return count;
    }
}

int PutMsg ( uint8_t buf[], size_t msgSize, FILE *out )
{
    //Check for delimiter in message
    int i;
    for ( i = 0; i < msgSize; i++ )
        if ( buf[i] == DELIMITER )
            return -1;
    if ( fwrite ( buf, 1, msgSize, out ) != msgSize )
        return -1;
    fputc ( DELIMITER, out );
    fflush ( out );
    return msgSize;
}
