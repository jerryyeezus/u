#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <dirent.h>
#include <string.h>	  /* support any string ops */
#include "musicProtocol.h"	/* declarations of necessary functions and data types */

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define FILEBUFSIZE 512		/* The file buffer size */
#define MAXPENDING 5

#define SERVER_DIR "./repo/"

/**
* get_files
*
* @param  dir FILE pointer
* @param  buffer Pre-allocated buffer that will be modified
* @return buffer gets filenames
*/
void get_files ( DIR *dir, struct dirent *ent, char *buffer )
{
    if ( ( dir = opendir ( "./repo" ) ) != NULL )
    {
        while ( ( ent = readdir ( dir ) ) != NULL )
        {

            char *d_name = ent->d_name;
            if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 )
            {
                strcat ( buffer, d_name );
                strcat ( buffer, "\n" );
            }
        }

        closedir ( dir );
    }

    strcat ( buffer, "\0" );
}

/* The main function */
int main ( int argc, char *argv[] )
{
    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */
    DIR *dir;
    struct dirent *ent;
    char filepath[FILENAME_MAX];
    char filename[FILENAME_MAX];
    char rcvBuf[RCVBUFSIZE];
    char sndBuf[SNDBUFSIZE];
    char fileBuf[FILEBUFSIZE];
    char serverFiles[4096];
    char *curFile;
    MusicInfo rcvInfo;
    memset(&rcvInfo, 0, sizeof(rcvInfo));
    MusicInfo sndInfo;
    memset(&sndInfo, 0, sizeof(sndInfo));

    /* Create new TCP Socket for incoming requests*/
    if ( ( serverSock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
    {
        exit ( 1 );
    }

    int on = 1;
    setsockopt ( serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
    memset ( &changeServAddr, 0, sizeof ( changeServAddr ) );
    changeServAddr.sin_family = AF_INET;
    changeServAddr.sin_addr.s_addr = htonl ( INADDR_ANY );
    changeServAddr.sin_port = htons ( 12003 );

    /* Bind to local address structure */
    if ( bind ( serverSock, ( struct sockaddr * ) &changeServAddr, sizeof ( changeServAddr ) ) < 0 )
    {
        perror ( "bind() failed." );
        exit ( 1 );
    }

    /* Listen for incoming connections */
    if ( listen ( serverSock, MAXPENDING ) < 0 )
    {
        perror ( "listen() failed." );
        exit ( 1 );
    }

    //char *rcvBuf = ( char * ) malloc ( RCVBUFSIZE );
    //char *sndBuf = ( char * ) malloc ( SNDBUFSIZE );

    /* Loop server forever*/
    while ( 1 )
    {
        /* Accept incoming connection */
        clientSock = accept ( serverSock, ( struct sockaddr * ) &changeClntAddr, &clntLen );
        if ( clientSock < 0 )
        {
            perror ( "accept() failed." );
            exit ( 1 );
        }

        int test = 0;
        while ( 1 )
        {
            memset ( rcvBuf, 0, RCVBUFSIZE );
            memset ( sndBuf, 0, SNDBUFSIZE );
	    memset(fileBuf, 0, FILEBUFSIZE);
            recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );

            /* Case list */

    	    if(Decode(rcvBuf, RCVBUFSIZE, &rcvInfo)) {
		printf("Request Type: %s\n", rcvInfo.requestType);
	    }
	    printf("Strcmp with list: %d\n", strcmp(rcvInfo.requestType, "list"));
	    printf("Strcmp with diff: %d\n", strcmp(rcvInfo.requestType, "diff"));
	    printf("Strcmp with pull: %d\n", strcmp(rcvInfo.requestType, "pull"));
	    printf("Request Type: %s\n", rcvInfo.requestType);
	    printf("Song Names: %s\n", rcvInfo.songNames);
	    printf("Song IDs: %s\n", rcvInfo.songIDs);
	    printf("End of file? %c\n", rcvInfo.eof);
	    printf("Terminate? %c\n", rcvInfo.terminate);

            if ( strcmp ( rcvInfo.requestType, "list" ) == 0 )
            {
		printf("Inside if statement\n");
                if ( ( dir= opendir ( "./repo" ) ) != NULL )
                {
                    while ( ( ent = readdir ( dir ) ) != NULL )
                    {

                        char *d_name = ent->d_name;
                        if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 )
                        {
                            strcat ( sndInfo.songNames, d_name );
                            strcat ( sndInfo.songNames, "|" );
                        }
                    }

                    closedir ( dir );
                }

                //strcat ( sndBuf, "\0" );
		strcpy(sndInfo.requestType, rcvInfo.requestType);
	        strcpy(sndInfo.songIDs, " ");
	        strcpy(sndInfo.fileData, " ");
		sndInfo.eof = 1;
		sndInfo.terminate = 1;
		size_t responseSize = Encode(&sndInfo, sndBuf, SNDBUFSIZE);
		printf("sndBuf: %s\n", sndBuf);
                send ( clientSock, sndBuf, SNDBUFSIZE, 0 );
            }	// end of list


	    else if ( strcmp ( rcvInfo.requestType, "diff" ) == 0 )
            {
                if ( ( dir= opendir ( "./repo" ) ) != NULL )
                {
                    while ( ( ent = readdir ( dir ) ) != NULL )
                    {

                        char *d_name = ent->d_name;
                        if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 )
                        {
                            strcat ( sndInfo.songNames, d_name );
                            strcat ( sndInfo.songNames, "|" );
                        }
                    }

                    closedir ( dir );
                }

                //strcat ( sndBuf, "\0" );
		strcpy(sndInfo.requestType, rcvInfo.requestType);
	        strcpy(sndInfo.songIDs, " ");
	        strcpy(sndInfo.fileData, " ");
		sndInfo.eof = 1;
		sndInfo.terminate = 1;
		size_t responseSize = Encode(&sndInfo, sndBuf, SNDBUFSIZE);
		printf("sndBuf: %s\n", sndBuf);
                send ( clientSock, sndBuf, SNDBUFSIZE, 0 );
            }	// end of list
	    /*else if ( strcmp ( rcvInfo.requestType, "diff" ) == 0 ) {
		printf("Diff requested\n");
		fflush(stdout);
		get_files(dir, ent, serverFiles);
		
	    }*/

            /* Case pull */
            else if ( strcmp ( rcvInfo.requestType, "pull" ) == 0 )
            {
                /* Get array of file names */
                get_files ( dir, ent, serverFiles );

                curFile = strtok ( serverFiles, "\n" );
                while ( curFile != NULL )
                {
                    printf ( "curFile = %s\n", curFile );
                    memset ( sndBuf, 0, SNDBUFSIZE );
                    memset ( filename, 0, FILENAME_MAX );
                    memset ( filepath, 0, FILENAME_MAX );

                    strcat ( filename, curFile );
                    strcat ( filepath, SERVER_DIR );
                    strcat ( filepath, filename );
                    FILE *fp = fopen ( filepath, "r" );

                    /* Send file name first */
                    strcat ( sndBuf, filename );
                    send ( clientSock, sndBuf, SNDBUFSIZE, 0 );

                    if ( fp != NULL )
                    {
                        /* Read the file into sndBuf */
                        while ( fread ( sndBuf, sizeof ( char ), SNDBUFSIZE, fp ) > 0 )
                        {
                            send ( clientSock, sndBuf, SNDBUFSIZE, 0 );
                            memset ( sndBuf, 0, SNDBUFSIZE );
                        }

                        /* Designate end of file */
                        send ( clientSock, "\0", 1, 0 );

                        fclose ( fp );
                    }

                    /* Get next one */
                    curFile = strtok ( NULL, "\n" );
                }
                printf ( "Done sending files.\n" );

            }	// end of pull
        }

        close ( clientSock );
    }

    free ( rcvBuf );
    free ( sndBuf );

}
