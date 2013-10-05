#include "musicProtocol.h"

#define RCVBUFSIZE 512

size_t Encode(const MusicInfo *music, uint8_t *outBuf, const size_t bufSize) {
    uint8_t *bufPtr = outBuf;
    long size = (size_t) bufSize;
    int musicPrint = snprintf((char *) bufPtr, size, "%s\t%s\t%s\t%s\t%c\t%c\n", music->requestType, music->songNames, music->songIDs, music->fileData, music->eof, music->terminate);
    bufPtr += musicPrint;
    size -= musicPrint;
    return (size_t) (bufPtr - outBuf);
}

bool Decode(uint8_t *inBuf, const size_t mSize, MusicInfo *music) {
    char *token;
    token = strtok((char *) inBuf, DELIM_INFO);
    //printf("Token: %s\n", token);
    if(token == NULL || (strcmp(token, LIST) != 0 && strcmp(token, DIFF) != 0 && strcmp(token, PULL) != 0 && strcmp(token, LEAVE) != 0)) {
	printf("Command not recognized\n");
	return false;
    }
    //printf("Command accepted\n");
    strcpy(music->requestType, token);

    token = strtok(NULL, DELIM_INFO);
    if(token == NULL)
	return false;
    
    //printf("Reached songNames token\n");
    strcpy(music->songNames, token);

    token = strtok(NULL, DELIM_INFO);
    if(token == NULL)
	return false;

    //printf("Reached songIDs token\n");
    strcpy(music->songIDs, token);

    token = strtok(NULL, DELIM_INFO);
    if(token == NULL)
	return false;
    
    //printf("Reached fileData token\n");
    strcpy(music->fileData, token);

    token = strtok(NULL, DELIM_INFO);
    if(token == NULL)
	return false;

    music->eof = token[0];
    //printf("EOF? %d\n", token[0]);

    token = strtok(NULL, DELIM_INFO);
    if(token == NULL)
	return false;

    music->terminate = token[0];
    //printf("Terminate? %d\n", token[0]);

    return true;
}
	

/*int main(int argc, char *argv[]) {

    MusicInfo musicIn;
    memset(&musicIn, 0, sizeof(musicIn));
    musicIn.requestType = "diff";
    musicIn.songNames = "Song 1|Song 2|Song 3";
    musicIn.songIDs = "ID 1|ID 2|ID 3";
    musicIn.isRequest = "R";
    uint8_t outbuf[1024];
    size_t reqSize = Encode(&musicIn, outbuf, 1024);
    printf("Bytes encoded: %d\n", reqSize);
    printf("Outbuf: %s\n", outbuf);

    MusicInfo musicOut;
    memset(&musicOut, 0, sizeof(musicOut));
    uint8_t inBuf[1024];
    bool decodeSuccess = Decode(outbuf, reqSize, &musicOut);
    printf("Request type: %s\n", musicOut.requestType);
    printf("Songs: %s\n", musicOut.songNames);
    printf("IDs: %s\n", musicOut.songIDs);


}*/
   
    
