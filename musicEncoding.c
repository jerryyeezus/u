#include "musicProtocol.h"

#define RCVBUFSIZE 512

size_t Encode ( const MusicInfo *music, uint8_t *outBuf, const size_t bufSize )
{
    uint8_t *bufPtr = outBuf;
    long size = ( size_t ) bufSize;
    int musicPrint = snprintf ( ( char * ) bufPtr, size, "%s\t%s\t%s\t%d\t%d\t%d\t", music->requestType, music->songNames, music->songIDs, music->eof, music->terminate, music->dataLen );
    int i;
    int len = strlen ( bufPtr );

    if ( strcmp ( "pull", music->requestType ) == 0 )
    {
        for ( i = 0; i < 512; i++ )
        {
            //    printf ( "%02X", music->fileData[i] );
        }
    }


    memcpy ( strlen ( bufPtr ) + bufPtr, music->fileData, music->dataLen );

    if ( strcmp ( "pull", music->requestType ) == 0 )
    {
        for ( i = 0; i < 512; i++ )
        {
            printf ( "%02X", bufPtr[i + len] );
        }
    }

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

    strcpy ( music->requestType, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    strcpy ( music->songNames, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    strcpy ( music->songIDs, token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    music->eof = token[0] == 49 ? 1 : 0;

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    music->terminate = token[0] == 49? 1 : 0;

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    music->dataLen = atoi ( token );

    token = strtok ( NULL, DELIM_INFO );
    if ( token == NULL )
        return false;
    memcpy ( music->fileData, token, music->dataLen );

    return true;
}


