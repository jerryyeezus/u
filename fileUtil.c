#include <sys/stat.h>
#include "musicProtocol.h"
#include "fileUtil.h"
#include <string.h>

int doDiffServer ( msg_t *rcvInfo, msg_t *sndInfo )
{
    int i = 0;
    DIR *dir;
    struct dirent *ent;
    char tmpStr[128];
    char filepath[FILENAME_MAX ];
    unsigned checksum;

    strcpy ( sndInfo->request, rcvInfo->request );
    if ( ( ( dir = opendir ( "./repo" ) ) != NULL ) )
    {
        while (  ( ent = readdir ( dir ) ) != NULL )
        {
            char *d_name = ent->d_name;

            memset ( filepath, 0, FILENAME_MAX );
            strcat ( filepath, SERVER_DIR );
            strcat ( filepath, d_name );
            checksum = 0;

            if ( *d_name != '.' )
            {
                strcpy ( sndInfo->filenames[i], d_name );

                FILE *fp = fopen ( filepath, "rb" );
                while ( !feof ( fp ) && !ferror ( fp ) )
                {
                    memset ( &tmpStr, 0, sizeof ( tmpStr ) );
                    fgets ( tmpStr, 128, fp );
                    checksum += atoi ( tmpStr );
                }
                fclose ( fp );
                sndInfo->cksums[i++] = checksum;
            }
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
    int cksumBuf[32];
    char tmpStr[128];
    char filepath[FILENAME_MAX];
    struct stat s;
    unsigned checksum = 0;

    /* Get files on client first */
    if ( ( dir = opendir ( UZIC_DIR ) ) != NULL )
    {
        while ( ( ent = readdir ( dir ) ) != NULL )
        {
            char *d_name = ent->d_name;
            memset ( filepath, 0, FILENAME_MAX );
            strcat ( filepath, UZIC_DIR );
            strcat ( filepath, d_name );
            stat ( filepath, &s );
            checksum = 0;

            if ( *d_name != '.' )
            {
                strcpy ( fnBuf[numClientFiles], d_name );
                FILE *fp = fopen ( filepath, "rb" );
                while ( !feof ( fp ) && !ferror ( fp ) )
                {
                    memset ( &tmpStr, 0, sizeof ( tmpStr ) );
                    fgets ( tmpStr, 128, fp );
                    checksum += atoi ( tmpStr );
                }
                fclose ( fp );
                cksumBuf[numClientFiles++] = checksum;
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
            if ( rcvInfo->cksums[i] == cksumBuf[j] )
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
