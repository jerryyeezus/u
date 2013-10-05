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
#define RCVBUFSIZE 512		    /* The receive buffer size */
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

    char sndBuf[SNDBUFSIZE];
    char rcvBuf[RCVBUFSIZE];
    char curFile[FILENAME_MAX];
    MusicInfo sndInfo;
    MusicInfo rcvInfo;
    MusicInfo clientInfo;
    DIR *dir;
    struct dirent *ent;

    int i;			    /* Counter Value */

    memset ( &sndBuf, 0, RCVBUFSIZE );
    memset ( &rcvBuf, 0, RCVBUFSIZE );
    memset(&sndInfo, 0, sizeof(sndInfo));
    memset(&rcvInfo, 0, sizeof(rcvInfo));
    memset(&clientInfo, 0, sizeof(clientInfo));

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

    /*FILE *musicStream = fdopen(ret, "r+"); /////////
    if(str == NULL) {
	fprintf("fdopen() failed\n");
	exit(1);
    }
    else
	printf("fdopen() success\n");*/

    printf ( "Welcome to uZic! Type list, diff, pull to proceed.\n" );
    char input[SNDBUFSIZE];
    while ( 1 )
    {
        memset ( &rcvBuf, 0, RCVBUFSIZE );
        printf ( ">> " );
        gets ( input );
	if(input) {
	    strcpy(sndInfo.requestType, input);
	    strcpy(sndInfo.songNames, " ");
	    strcpy(sndInfo.songIDs, " ");
	    strcpy(sndInfo.fileData, " ");
	    sndInfo.eof = 1;
	    sndInfo.terminate = 1;
	    size_t reqSize = Encode(&sndInfo, sndBuf, SNDBUFSIZE);
	    //printf("Bytes encoded: %d\n", reqSize);
	}
	//printf("sndBuf: %s\n", sndBuf);

        /* Send command to server */
        send ( clientSock, sndBuf, SNDBUFSIZE, 0 );

        /* Get file name */

	recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
	if(Decode(rcvBuf, RCVBUFSIZE, &rcvInfo))
	    printf("Response received for request: %s\n\n", rcvInfo.requestType);

	/* Case list */
	if(strcmp(rcvInfo.requestType, "list") == 0) {
	    printf("Songs in server library:\n");
	    char tmpBuf[TMPBUFSIZE];
	    strcpy(tmpBuf, rcvInfo.songNames);
	    char* token;
	    for(token = strtok(tmpBuf, "|"); token; token = strtok(NULL, "|"))
		printf("%s\n", token);
	}
	
	/* Case diff */
	if(strcmp(rcvInfo.requestType, "diff") == 0) {

	    //Set up struct for local file info
	    strcpy(clientInfo.requestType, rcvInfo.requestType);
	    strcpy(clientInfo.songIDs, " ");
	    strcpy(clientInfo.fileData, " ");
	    clientInfo.eof = 1;
	    clientInfo.terminate = 1;

	    //Get local file names
            if ( ( dir= opendir ( UZIC_DIR ) ) != NULL ) {
                while ( ( ent = readdir ( dir ) ) != NULL ) {
                    char *d_name = ent->d_name;
                    if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 ) {
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
	    char diffBuf[TMPBUFSIZE];
	    memset(&tmpBuf1, 0, sizeof(tmpBuf1));
	    memset(&tmpBuf2, 0, sizeof(tmpBuf2));
	    memset(&matchBuf, 0, sizeof(matchBuf));
	    memset(&diffBuf, 0, sizeof(diffBuf));
	    memset(&tokBuf1, 0, sizeof(tokBuf1));
	    memset(&tokBuf1, 0, sizeof(tokBuf2));
	    strcpy(tmpBuf1, rcvInfo.songNames);
	    
	    //Iterate through client & server songs, checking for matches and placing differences in a delimited string
	    for(token1 = strtok_r((char *) tmpBuf1, DELIM_SONG, &tokBuf1); token1; token1 = strtok_r(NULL, DELIM_SONG, &tokBuf1)) {
		//printf("Looping through 1, Token 1 = %s\n", token1);
	    	memset(&tmpBuf2, 0, sizeof(tmpBuf2));
		strcpy(tmpBuf2, clientInfo.songNames);
		for(token2 = strtok_r((char *) tmpBuf2, DELIM_SONG, &tokBuf2); token2; token2 = strtok_r(NULL, DELIM_SONG, &tokBuf2)) {
		    //printf("Looping through 2, Token 2 = %s\n", token2);
		    if(strcmp(token1, token2) == 0) {
			//printf("Found Match\n");
			strcat(matchBuf, token1);
			strcat(matchBuf, "|");
			match = 1;
			break;
		    }
		}
		if(match == 0) {
		    strcat(diffBuf, token1);
		    strcat(diffBuf, "|");
		    //printf("No match found\n");
		}
		match = 0;
	    }

	    //Print out list of matches
	    memset(&tmpBuf1, 0, sizeof(tmpBuf1));
	    strcpy(tmpBuf1, matchBuf);
	    printf("On both client and server:\n");
	    for(token1 = strtok(tmpBuf1, "|"); token1; token1 = strtok(NULL, "|"))
		printf("%s\n", token1);

	    //Print out list of diffs
	    memset(&tmpBuf2, 0, sizeof(tmpBuf2));
	    strcpy(tmpBuf2, diffBuf);
	    printf("\nOn server but not client:\n");
	    for(token2 = strtok(tmpBuf2, "|"); token2; token2 = strtok(NULL, "|"))
		printf("%s\n", token2);
	}

	if(strcmp(rcvInfo.requestType, "pull") == 0) {

	}

	if(strcmp(rcvInfo.requestType, "leave") == 0) {

	}

        /*if ( strcmp ( input, "pull" ) == 0 )
        {
            strcat ( rcvBuf, UZIC_DIR );
            recv ( clientSock, rcvBuf + strlen ( UZIC_DIR ), RCVBUFSIZE, 0 );
            strcpy ( curFile, rcvBuf );
            fp = fopen ( curFile, "w" );
        }

        memset ( &rcvBuf, 0, RCVBUFSIZE );
        char receiving = 1;
        size_t bytesRcv;*/
        /*while ( ( bytesRcv = recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 ) ) > 0 )
        {*/

            /* Check for terminator in the current buffer 
            for ( i = 0; i < RCVBUFSIZE; i++ )
            {
                if ( rcvBuf[i] == '\0' )
                {
                    receiving = 0;
                    break;
                }
            }

            fwrite ( rcvBuf, sizeof ( char ), bytesRcv, fp );*/

            /* Break out of recv loop if terminator found 
            if ( !receiving )
            {
                if ( fp != NULL )
                {
                    fclose ( fp );*/

                    /* Get next file name if available 
                    memset ( &rcvBuf, 0, RCVBUFSIZE );
                    memset ( &curFile, 0, FILENAME_MAX );
                    recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );

                    if ( rcvBuf[0] !=  '\0' )
                    {
                        strcat ( curFile, UZIC_DIR );
                        strcat ( curFile, rcvBuf );
                        printf ( "curFile = %s\n", curFile );
                        fp = fopen ( curFile, "w" );
                    }
                    else
                    {
                        printf ( "File transfer complete.\n" );
                        break;
                    }
                }
                else
                    break;
            }

            memset ( &rcvBuf, 0, RCVBUFSIZE );
        }*/


    }

    close ( clientSock );
    return 0;
}

