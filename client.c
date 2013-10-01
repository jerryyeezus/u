/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 5		    /* The send buffer size */

char *servIP = "localhost";

/* The main function */
int main ( int argc, char *argv[] )
{
    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */

    char sndBuf[SNDBUFSIZE];
    char rcvBuf[RCVBUFSIZE];

    int i;			    /* Counter Value */

    memset ( &sndBuf, 0, RCVBUFSIZE );
    memset ( &rcvBuf, 0, RCVBUFSIZE );

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
    char input[5];
    while ( 1 )
    {
        memset ( &rcvBuf, 0, RCVBUFSIZE );
        printf ( ">> " );
        gets ( input );

        /* Send command to server */
        send ( clientSock, input, 5, 0 );

        /* Get file name */
        if ( strcmp ( input, "pull" ) == 0 )
        {
            recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 );
            printf ( "rcvBuf = %s\n", rcvBuf );
        }

        memset ( &rcvBuf, 0, RCVBUFSIZE );
        char receiving = 1;
        size_t bytesRcv;
        while ( ( bytesRcv = recv ( clientSock, rcvBuf, RCVBUFSIZE, 0 ) ) > 0 )
        {
            printf ( "loop\n" );

            /* Check for terminator in the current buffer */
            for ( i = 0; i < RCVBUFSIZE; i++ )
            {
                if ( rcvBuf[i] == '\0' )
                {
                    printf ( "i = %d\n", i );
                    receiving = 0;
                    break;
                }
            }
            printf ( "rcvBuf = %s\n", rcvBuf );

            /* Break out of recv loop if terminator found */
            if ( !receiving )
                break;

            memset ( &rcvBuf, 0, RCVBUFSIZE );
        }

    }

    close ( clientSock );
    return 0;
}

