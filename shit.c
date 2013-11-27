#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define TAG_NAME "<key>Name</key><string>"
#define TAG_SIZE "<key>Size"
#define TAG_COUNT "<key>Play Count"

typedef struct capInfo
{
    char name[FILENAME_MAX];
    int size;
    int playCount;
    struct capInfo *next;
} capInfo;


void insertInOrder ( capInfo **capNodes, capInfo **new )
{
    capInfo *cur = *capNodes;
    /* Case head is NULL */
    if ( cur == NULL )
    {
        *capNodes = *new;
        ( *new )->next = NULL;
        return;
    }

    /* Case bigger than HEAD */
    if ( ( *new )->playCount > cur->playCount )
    {
        capInfo *tmp = *capNodes;
        *capNodes = *new;
        ( *new )->next = tmp;
        return;
    }

    /* Case insert in between */
    while ( cur->next != NULL && cur->next->playCount > ( *new )->playCount )
    {
        cur = cur->next;
    }

    capInfo *tmp = cur->next;
    cur->next = *new;
    ( *new )->next = tmp;
}

char *getNextNode ( char **ptr, const char *tag )
{
    int i = 0;
    char *ret = malloc ( FILENAME_MAX );;
    memset ( ret, 0, FILENAME_MAX );
    *ptr = strstr ( *ptr, tag );
    if ( *ptr == NULL )
        return NULL;

    *ptr = strstr ( ++ ( *ptr ), ">" );
    *ptr = strstr ( ++ ( *ptr ), ">" );
    *ptr = strstr ( ++ ( *ptr ), ">" );
    ++ ( *ptr );

    while ( ( *ptr ) [i] != '<' )
    {
        ret[i] = ( *ptr ) [i];
        i++;
    }

    return ret;
}

capInfo *getAllowedFiles()
{
    char *iTunesXML = "iTunes Music Library.xml";
    struct stat s;
    int i = 0;
    char *nameBuf;
    char *sizeBuf;
    char *cntBuf;
    capInfo *ptr;
    capInfo *capNodes = NULL;

    stat ( iTunesXML, &s );
    FILE *fp = fopen ( iTunesXML, "r" );
    char *xmlBuffer = malloc ( s.st_size );

    int totBytesRead = 0, bytesRead = 0;
    while ( totBytesRead < s.st_size )
    {
        bytesRead = fread ( xmlBuffer + totBytesRead, 1, 1024, fp )	;
        totBytesRead += bytesRead;
    }
    fclose ( fp );

    char *nextSong = xmlBuffer;

    // Looping
    while ( nextSong != NULL )
    {
        nameBuf = getNextNode ( &nextSong, TAG_NAME );
        sizeBuf = getNextNode ( &nextSong, TAG_SIZE );
        cntBuf = getNextNode ( &nextSong, TAG_COUNT );
        if ( cntBuf != NULL )
        {
            capInfo *new = malloc ( sizeof ( capInfo ) );
            strcpy ( new->name, nameBuf );
            new->size = atoi ( sizeBuf );
            new->playCount = atoi ( cntBuf );
            insertInOrder ( &capNodes, &new );
        }

        free ( nameBuf );
        free ( sizeBuf );
        free ( cntBuf );
    }

    free ( xmlBuffer );
    return capNodes;
}

void freeNodes ( capInfo **capNodes )
{
    capInfo *ptr = *capNodes;
    while ( *capNodes != NULL )
    {
        ptr = ( *capNodes )->next;
        free ( *capNodes );
        *capNodes = ptr;
    }

}

int main ( int argc, const char *argv[] )
{
    capInfo *capNodes = getAllowedFiles();

    capInfo *ptr = capNodes;

    while ( ptr != NULL )
    {
        printf ( "ptr->name = %s\n", ptr->name );
        printf ( "ptr->playCount = %d\n", ptr->playCount );
        ptr = ptr->next;
    }

    freeNodes ( &capNodes );
    return 0;
}
