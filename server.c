#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <dirent.h>
#include <string.h>	  /* support any string ops */

#define RCVBUFSIZE 5		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define MAXPENDING 5

#define SERVER_DIR "~"

char *rcvBuf;
char *sndBuf;
char *fileBuf;

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

    char *rcvBuf = ( char * ) malloc ( RCVBUFSIZE );
    char *sndBuf = ( char * ) malloc ( SNDBUFSIZE );

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

        while ( 1 )
        {
            memset ( rcvBuf, 0, RCVBUFSIZE );
            memset ( sndBuf, 0, SNDBUFSIZE );
            recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );

            /* Case list */
            if ( strcmp ( rcvBuf, "list" ) == 0 )
            {
                strcpy ( sndBuf, "list" );
                if ( ( dir= opendir ( "./repo" ) ) != NULL )
                {
                    while ( ( ent = readdir ( dir ) ) != NULL )
                    {

                        char *d_name = ent->d_name;
                        if ( *d_name != '.' && strcmp ( d_name, ".." ) != 0 )
                        {
                            strcat ( sndBuf, d_name );
                            strcat ( sndBuf, "\n" );
                        }
                    }

                    closedir ( dir );
                }

                strcat ( sndBuf, "\0" );
                send ( clientSock, sndBuf, SNDBUFSIZE, 0 );
            }	// end of list

            /* Case pull */
            else if ( strcmp ( rcvBuf, "pull" ) == 0 )
            {
                memset ( sndBuf, 0, SNDBUFSIZE );
                char dirfile[256];
                strcat ( dirfile, "./repo/Kesha.mp3" );
                FILE *fp = fopen ( "/home/jy/Dropbox/workspace/3251/Projects/2/repo/Kesha.mp3", "r" );

                /* Send file name first */
                strcat ( sndBuf, "Kesha.mp3" );
                send ( clientSock, sndBuf, SNDBUFSIZE, 0 );

                /* fopen was successful */
                if ( fp != NULL )
                {
                    /* Read the file into sndBuf */
                    while ( fread ( sndBuf, sizeof ( char ), SNDBUFSIZE, fp ) > 0 )
                    {
                        send ( clientSock, sndBuf, SNDBUFSIZE, 0 );
                    }
                    fclose ( fp );
                }


            }	// end of pull
        }

        close ( clientSock );
    }

    free ( rcvBuf );
    free ( sndBuf );

}
