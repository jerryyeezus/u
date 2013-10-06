/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "musicProtocol.h"	/* declarations of necessary functions and data types */

/* Constants */
#define RCVBUFSIZE 2048		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define TMPBUFSIZE 512		    /* Size for temporary buffers */

#define UZIC_DIR "./my_uZic/"

char *servIP = "localhost";

/* The main function */
int main ( int argc, char *argv[] )
{
    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */

    FILE *fp = NULL;

    char diffBuf[TMPBUFSIZE];
    char sndBuf[SNDBUFSIZE];
    char rcvBuf[RCVBUFSIZE];
    char curFile[FILENAME_MAX];
    MusicInfo sndInfo;
    MusicInfo rcvInfo;
    MusicInfo clientInfo;
    DIR *dir;
    struct dirent *ent;

    int i;			    /* Counter Value */

    memset ( &sndBuf, 0, SNDBUFSIZE );
    memset ( &rcvBuf, 0, RCVBUFSIZE );
    memset ( diffBuf, 0, TMPBUFSIZE );
    memset ( &sndInfo, 0, sizeof ( sndInfo ) );
    memset ( &rcvInfo, 0, sizeof ( rcvInfo ) );
    memset ( &clientInfo, 0, sizeof ( clientInfo ) );

    /* Create a new TCP socket*/
    clientSock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( clientSock < 0 )
        fprintf ( stderr, "Fail to initialize socket\n" );

    memset ( &serv_addr, 0, sizeof ( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    int ret = inet_pton ( AF_INET, servIP, &serv_addr.sin_addr.s_addr );
    serv_addr.sin_port = htons ( 12003 );

    /* Establish connecction to the server */
    if ( ( ret = connect ( clientSock, ( struct sockaddr * ) &serv_addr, sizeof ( serv_addr ) ) ) < 0 )
    {
        fprintf ( stderr, "connection failed\n" );
        exit ( 1 );
    }

    printf ( "Welcome to uZic! Type list, diff, pull to proceed.\n" );
    char input[SNDBUFSIZE];
    while ( 1 )
    {
        memset ( &rcvBuf, 0, RCVBUFSIZE );
        memset ( &sndBuf, 0, SNDBUFSIZE );
        memset ( input, 0, SNDBUFSIZE );
        printf ( ">> " );
        gets ( input );
        if ( input )
        {
            strcpy ( sndInfo.requestType, input );
            strcpy ( sndInfo.songNames, " " );
            //strcpy ( sndInfo.songIDs, " " );
            strcpy ( sndInfo.songIDs, diffBuf );
            strcpy ( sndInfo.fileData, " " );
            sndInfo.eof = 1;
            sndInfo.terminate = 1;
            size_t reqSize = Encode ( &sndInfo, sndBuf, SNDBUFSIZE );
        }

        /* Send command to server */
        send ( clientSock, sndBuf, SNDBUFSIZE, 0 );

        /* Get server response */
        recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );
        if ( Decode ( rcvBuf, RCVBUFSIZE, &rcvInfo ) )
            printf ( "Response received for request: %s\n\n", rcvInfo.requestType );

        /* Case list */
        if ( strcmp ( rcvInfo.requestType, "list" ) == 0 )
        {
            printf ( "Songs in server library:\n" );
            char tmpBuf[TMPBUFSIZE];
            strcpy ( tmpBuf, rcvInfo.songNames );
            char* token;
            for ( token = strtok ( tmpBuf, "|" ); token; token = strtok ( NULL, "|" ) )
                printf ( "%s\n", token );
        }

        /* Case diff */
        if ( strcmp ( rcvInfo.requestType, "diff" ) == 0 )
        {
            //Set up struct for local file info
            strcpy ( clientInfo.requestType, rcvInfo.requestType );
            strcpy ( clientInfo.songIDs, " " );
            strcpy ( clientInfo.fileData, " " );
            clientInfo.eof = 1;
            clientInfo.terminate = 1;

            //Get local file names
            if ( ( dir= opendir ( UZIC_DIR ) ) != NULL )
            {
                while ( ( ent = readdir ( dir ) ) != NULL )
                {
                    char *d_name = ent->d_name;
                    if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 )
                    {
                        strcat ( clientInfo.songNames, d_name );
                        strcat ( clientInfo.songNames, "|" );
                    }
                }
                closedir ( dir );
            }

            //Initialize all buffers for strtok
            char tmpBuf1[TMPBUFSIZE];
            char* token1;
            char* tokBuf1;
            char tmpBuf2[TMPBUFSIZE];
            char* token2;
            char* tokBuf2;
            char matchBuf[TMPBUFSIZE];
            int match = 0;
            memset ( &tmpBuf1, 0, sizeof ( tmpBuf1 ) );
            memset ( &tmpBuf2, 0, sizeof ( tmpBuf2 ) );
            memset ( &matchBuf, 0, sizeof ( matchBuf ) );
            memset ( &diffBuf, 0, sizeof ( diffBuf ) );
            memset ( &tokBuf1, 0, sizeof ( tokBuf1 ) );
            memset ( &tokBuf1, 0, sizeof ( tokBuf2 ) );
            strcpy ( tmpBuf1, rcvInfo.songNames );

            //Iterate through client & server songs, checking for matches and placing differences in a delimited string
            for ( token1 = strtok_r ( ( char * ) tmpBuf1, DELIM_SONG, &tokBuf1 ); token1; token1 = strtok_r ( NULL, DELIM_SONG, &tokBuf1 ) )
            {
                //printf("Looping through 1, Token 1 = %s\n", token1);
                memset ( &tmpBuf2, 0, sizeof ( tmpBuf2 ) );
                strcpy ( tmpBuf2, clientInfo.songNames );
                for ( token2 = strtok_r ( ( char * ) tmpBuf2, DELIM_SONG, &tokBuf2 ); token2; token2 = strtok_r ( NULL, DELIM_SONG, &tokBuf2 ) )
                {
                    //printf("Looping through 2, Token 2 = %s\n", token2);
                    if ( strcmp ( token1, token2 ) == 0 )
                    {
                        strcat ( matchBuf, token1 );
                        strcat ( matchBuf, "|" );
                        match = 1;
                        break;
                    }
                }
                if ( match == 0 )
                {
                    strcat ( diffBuf, token1 );
                    strcat ( diffBuf, "|" );
                }

                match = 0;
            }

            //Print out list of diffs
            memset ( &tmpBuf2, 0, sizeof ( tmpBuf2 ) );
            strcpy ( tmpBuf2, diffBuf );
            printf ( "\nOn server but not client:\n" );
            for ( token2 = strtok ( tmpBuf2, "|" ); token2; token2 = strtok ( NULL, "|" ) )
                printf ( "%s\n", token2 );
        }

        else if ( strcmp ( rcvInfo.requestType, "pull" ) == 0 )
        {
            char filepath[FILENAME_MAX];

            while ( 1 )
            {

                memset ( filepath, 0, sizeof ( filepath ) );
                strcpy ( filepath, UZIC_DIR );
                strcat ( filepath, rcvInfo.songNames );

                if ( fp == NULL )
                    fp = fopen ( filepath, "w" );

                printf ( "rcvInfo.fileData = %s\n", rcvInfo.fileData );
                fwrite ( rcvInfo.fileData, 1, strlen ( rcvInfo.fileData ), fp );

                if ( rcvInfo.eof )
                {
                    fclose ( fp );
                    fp = NULL;
                }

                if ( rcvInfo.terminate )
                    break;

                recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );
                Decode ( rcvBuf, RCVBUFSIZE, &rcvInfo );
            }
        }

        else if ( strcmp ( rcvInfo.requestType, "leave" ) == 0 )
        {

        }

        else
        {
            printf ( "Derp. Something screwed up. \n" );
        }

    }

    close ( clientSock );
    return 0;
}

