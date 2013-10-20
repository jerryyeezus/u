#include "musicProtocol.h"
#include "fileUtil.h"

int doDiffServer ( msg_t *rcvInfo, msg_t *sndInfo )
{
    int i = 0;
    DIR *dir;
    struct dirent *ent;

    strcpy ( sndInfo->request, rcvInfo->request );
    if ( ( ( dir = opendir ( "./repo" ) ) != NULL ) )
    {
        while (  ( ent = readdir ( dir ) ) != NULL )
        {
            char *d_name = ent->d_name;
            if ( *d_name != '.' )
                strcpy ( sndInfo->filenames[i++], d_name );
        }
    }

    return i;
}

int doDiffClient ( msg_t *rcvInfo, msg_t *sndInfo )
{
    DIR *dir;
    struct dirent *ent;
    int i;
    int j;
    int numClientFiles = 0;
    int numUnmatched = 0;
    char isFound;
    char fnBuf[128][FILENAME_MAX];

    /* Get files on client first */
    if ( ( dir = opendir ( UZIC_DIR ) ) != NULL )
    {
        while ( ( ent = readdir ( dir ) ) != NULL )
        {
            char *d_name = ent->d_name;
            if ( *d_name != '.' )
            {
                strcpy ( fnBuf[numClientFiles++], d_name );
            }
        }
    }

    /* Find diff of arrays */
    for ( i = 0; i < rcvInfo->len; i++ )
    {
        isFound = 0;
        for ( j = 0; j < numClientFiles; j++ )
        {
            /* Client filename match found */
            if ( strcmp ( rcvInfo->filenames[i], fnBuf[j] ) == 0 )
            {
                isFound = 1;
                break;
            }
        }

        if ( !isFound )
        {
            strcpy ( sndInfo->filenames[numUnmatched++], rcvInfo->filenames[i] );
        }
    }

    return numUnmatched;
}
