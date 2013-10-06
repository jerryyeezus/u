#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

static const char *LIST = "list";
static const char *DIFF = "diff";
static const char *PULL = "pull";
static const char *LEAVE = "leave";
static const char *REQUESTSTR = "Y";
static const char *DELIM_INFO = "\t";
static const char *DELIM_SONG = "|";

struct MusicInfo
{
    char requestType[6];
    char songNames[512];
    char songIDs[512];
    char eof;	//End of file
    char terminate;	//End of command
    int dataLen;
    char fileData[512];
};

typedef struct MusicInfo MusicInfo;

enum
{
    MAX_SONG_NAMES = 512,	//Maximum byte size of the songNames array
    MAX_SONG_IDS = 512		//Maximum byte size of the songIDs array
};

int GetNextMsg ( FILE *in, uint8_t *buf, size_t bufSize );
size_t Encode ( const MusicInfo *music, uint8_t *outBuf, const size_t bufSize );
bool Decode ( uint8_t *inBuf, const size_t mSize, MusicInfo *music );
bool getDiff(char* diffBuf, MusicInfo serverInfo);
