#include <sys/stat.h>
#include "musicProtocol.h"
#include "fileUtil.h"
#include <string.h>
#include <openssl/md5.h>

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
                /*unsigned char c[MD5_DIGEST_LENGTH];
                int j;

                FILE *fp = fopen ( filepath, "rb" );
                MD5_CTX mdContext;
                int bytes;
                unsigned char data[1024];
                
                if(fp == NULL){
                    printf("%s can't be opened.\n", filepath);
                    exit(1);
                }

                MD5_Init(&mdContext);
                while((bytes = fread(data, 1, 1024, fp)) != 0){
                    MD5_Update(&mdContext, data, bytes);
                }
                MD5_Final(c, &mdContext);
                for(j = 0; j < MD5_DIGEST_LENGTH; j++)
                    printf("%02x", c[j]);
                printf(" %s\n", filepath);
                fclose(fp);
                sndInfo->cksums[i++] = c;*/

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
