#include "musicProtocol.h"

#define RCVBUFSIZE 512

size_t Encode ( const MusicInfo *music, uint8_t *outBuf, const size_t bufSize )
{
    uint8_t *bufPtr = outBuf;
    long size = ( size_t ) bufSize;
    int musicPrint = snprintf ( ( char * ) bufPtr, size, "%s\t%s\t%s\t%s\t%c\t%c\n", music->requestType, music->songNames, music->songIDs, music->fileData, music->eof, music->terminate );
    bufPtr += musicPrint;
    size -= musicPrint;
    return ( size_t ) ( bufPtr - outBuf );
}

bool Decode ( uint8_t *inBuf, const size_t mSize, MusicInfo *music )
{
    char *token;
    token = strtok ( ( char * ) inBuf, DELIM_INFO );
    if ( token == NULL || ( strcmp ( token, LIST ) != 0 && strcmp ( token, DIFF ) != 0 && strcmp ( token, PULL ) != 0 && strcmp ( token, LEAVE ) != 0 ) )
    {
        printf ( "ERROR: Command not recognized\n" );
        return false;
    }
    //printf("Command accepted\n");
    strcpy ( music->requestType, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;

    //printf("Reached songNames token\n");
    strcpy ( music->songNames, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;

    //printf("Reached songIDs token\n");
    strcpy ( music->songIDs, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;

    //printf("Reached fileData token\n");
    strcpy ( music->fileData, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;

    music->eof = token[0];
    //printf("EOF? %d\n", token[0]);
    printf ( "music->eof = %d\n", music->eof );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;

    music->terminate = token[0];
    printf ( "music->terminate = %d\n", music->terminate );
    //printf("Terminate? %d\n", token[0]);

    return true;
}


