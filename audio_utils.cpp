#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vorbis/codec.h>   
#include <vorbis/vorbisfile.h> 

#include "magicsoundsystem_oggfile.h"

#define true 1
#define false 0
#define bool int

static double   mvolume = 1;

long MSS_LongSwap (char* buffer)
{

    long l = (*(long*)buffer);
	unsigned char    b1,b2,b3,b4;
	long lng=0;
	b1 = (unsigned char) l&0xff;
	b2 = (unsigned char) (l>>8)&0xff;
	b3 = (unsigned char) (l>>16)&0xff;
	b4 = (unsigned char) (l>>24)&0xff;
	lng=lng|(b1&255); lng=lng<<8;
	lng=lng|(b2&255); lng=lng<<8;
	lng=lng|(b3&255); lng=lng<<8;
	lng=lng|(b4&255);
	return lng; 
}

short MSS_ShortSwap (char* buffer)
{
    short l = (*(short*)buffer);

	unsigned char    b1,b2;
	short shrt=0;
	b1 = l&0xff;
	b2 = (l>>8)&0xff;

	shrt=shrt|(b1&255); shrt=shrt<<8;
	shrt=shrt|(b2&255);

	return shrt;
}

bool MSS_AnalyzeAIFFHeader(const char *filename, int *freq, int *channels, int *bitsPerSample, unsigned char **audioBuffer, unsigned int *audioLength) 
{
    FILE *file = fopen(filename, "rb");
    if (!file) 
    {
        return false;
    }

    // Check FORM chunk
    char chunkID[5];
    if (fread(chunkID, 1, 4, file) != 4 || strncmp(chunkID, "FORM", 4) != 0) 
    {
        fclose(file);
        return false;
    }

    // Read FORM chunk size
    unsigned int fileSize;
    if (fread(&fileSize, sizeof(unsigned int), 1, file) != 1) 
    {
        fclose(file);
        return false;
    }

    // Check AIFF format
    if (fread(chunkID, 1, 4, file) != 4 || strncmp(chunkID, "AIFF", 4) != 0) 
	{
        fclose(file);
        return false;
    }

    // Search for COMM chunk
    bool foundCOMM = false;
    while (!foundCOMM && ftell(file) < fileSize - 4) 
	{
        unsigned int chunkSize;
	
        // Read the chunk ID for the next iteration
        if (fread(chunkID, 1, 4, file) != 4) break;	
		
        if (fread(&chunkSize, sizeof(unsigned int), 1, file) != 1) break;

        if (strncmp(chunkID, "COMM", 4) == 0) 
		{
            foundCOMM = true;
            // Read channel count, sample rate, and sample size
            unsigned short channels16;
			unsigned int dummy;
            unsigned short sampleRate;
            unsigned short sampleSize;
            if (fread(&channels16, sizeof(unsigned short), 1, file) != 1 ||
                fread(&dummy, sizeof(unsigned int), 1, file) != 1 ||
                fread(&sampleSize, sizeof(unsigned short), 1, file) != 1 ||
				fread(&sampleRate, sizeof(unsigned short), 1, file) != 1)
				{
                fclose(file);
                return false;
            }
            *channels = (int)channels16;
            *freq = (int)sampleRate;
            *bitsPerSample = (int)sampleSize;
        } else 
		{
            fseek(file, chunkSize, SEEK_CUR);
        }
    }

    if (!foundCOMM) 
    {
        fclose(file);
        return false;
    }

    // Search for SSND chunk
    bool foundSSND = false;
    while (!foundSSND && ftell(file) < fileSize - 4) 
    {
        unsigned int chunkSize;		
		
        if (fread(chunkID, 1, 4, file) != 4) break;
        if (fread(&chunkSize, sizeof(unsigned int), 1, file) != 1) break;
        
        if (strncmp(chunkID, "SSND", 4) == 0) 
        {
			foundSSND = true;
        
			// Read offset (big-endian 32-bit integer)
			unsigned int offset;
			if (fread(&offset, sizeof(unsigned int), 1, file) != 1) 
			{
				fclose(file);
				return false;
			}
        
			// Read blockSize (big-endian 32-bit integer)
			unsigned int blockSize;
			if (fread(&blockSize, sizeof(unsigned int), 1, file) != 1) 
			{
				fclose(file);
				return false;
			}
        
			// Skip blockSize (if necessary)
			if (blockSize > 0) 
			{
				fseek(file, blockSize, SEEK_CUR);
			}
        
			// Calculate actual data size
			unsigned int dataSize = chunkSize - 8; // subtract 8 bytes for offset and blockSize
        
			// Allocate memory for audio data
			*audioBuffer = (unsigned char*)malloc(dataSize);
			if (!*audioBuffer) 
			{
				fclose(file);
				return false;
			}
        
			// Read audio data
			if (fread(*audioBuffer, sizeof(unsigned char), dataSize, file) != dataSize) 
			{
				fclose(file);
				free(*audioBuffer);
				return false;
			}
        
			*audioLength = dataSize;
        } 
        else 
        {
            // Skip other chunks
            fseek(file, chunkSize, SEEK_CUR);
        }
    }

    if (!foundSSND) 
    {
        fclose(file);
        return false;
    }

    fclose(file);
	
    return true;
}


bool MSS_AnalyzeWAVHeader(unsigned char* buffer, unsigned int size, int* freq, int* channels, int* bitsPerSample) 
{
    char* bufferPtr = (char*)(buffer);
    int bufferPos = 0;

    // Check for RIFF chunk
    if (strncmp(&bufferPtr[bufferPos], "RIFF", 4) != 0) 
	{
        return false;
    }

    // Move to the fmt subchunk
    bufferPos += 12; // Skip RIFF header and "WAVE" format tag

    // Search for the "fmt " subchunk
    while (bufferPos < size - 8) 
	{
        if (strncmp(&bufferPtr[bufferPos], "fmt ", 4) == 0) 
		{
            break; // Found the "fmt " subchunk
        }
        bufferPos++;
    }

    // Check if "fmt " subchunk was found
    if (bufferPos >= size - 8) 
	{
        return false;
    }

    // Move past the "fmt " subchunk ID and size
    bufferPos += 8;

    // Read audio format data
    *channels = MSS_ShortSwap(&bufferPtr[bufferPos + 2]);
    *freq = MSS_LongSwap(&bufferPtr[bufferPos + 4]);
    *bitsPerSample = MSS_ShortSwap(&bufferPtr[bufferPos + 14]);

    return true;
}

bool MSS_LoadAudioFromBuffer(unsigned char* buffer, unsigned int size, unsigned char** audioBuffer, unsigned int* audioLength) 
{
    char* bufferPtr = (char*)(buffer);
    int bufferPos = 0;

    // Check for RIFF chunk
    if (strncmp(&bufferPtr[bufferPos], "RIFF", 4) != 0) 
    {
        return false;
    }
    bufferPos += 8; // Skip "RIFF" and chunk size

    // Check for WAVE format
    if (strncmp(&bufferPtr[bufferPos], "WAVE", 4) != 0) 
    {
        return false;
    }
    bufferPos += 4;

    // Find the "fmt " subchunk
    while (bufferPos < size) 
    {
        if (strncmp(&bufferPtr[bufferPos], "fmt ", 4) == 0) 
        {
            bufferPos += 8; // Skip "fmt " and subchunk size
            break;
        }
        bufferPos += 4;
        unsigned int chunkSize = MSS_LongSwap(&bufferPtr[bufferPos]);
        bufferPos += 4 + chunkSize; // Skip chunk size and chunk data
    }

    if (bufferPos >= size) 
    {
        return false;
    }

    bufferPos += 2; // Skip audio format
    short channels = MSS_ShortSwap(&bufferPtr[bufferPos]);
    bufferPos += 2;
    int freq = MSS_LongSwap(&bufferPtr[bufferPos]);
    bufferPos += 4;
    bufferPos += 6; // Skip byte rate and block align
    short bits = MSS_ShortSwap(&bufferPtr[bufferPos]);
    bufferPos += 2;

    // Find the "data" subchunk
    while (bufferPos < size) 
    {
        if (strncmp(&bufferPtr[bufferPos], "data", 4) == 0) 
        {
            bufferPos += 8; // Skip "data" and subchunk size
            break;
        }
        bufferPos += 4;
        unsigned int chunkSize = MSS_LongSwap(&bufferPtr[bufferPos]);
        bufferPos += 4 + chunkSize; // Skip chunk size and chunk data
    }

    if (bufferPos >= size) 
    {
        return false;
    }

    unsigned int dataSize = MSS_LongSwap(&bufferPtr[bufferPos - 4]);
    *audioBuffer = (unsigned char*)(&bufferPtr[bufferPos]);
    *audioLength = dataSize;
	
	unsigned char* audioData = (unsigned char*)malloc(dataSize);
    if (!audioData) 
    {
        fprintf(stderr, "ERROR: Could not allocate memory for audio data\n");
        return false;
    }
    memcpy(audioData, *audioBuffer, dataSize);

    *audioBuffer = audioData;	

    return true;
}

size_t MSS_readOgg(void* dst, size_t size1, size_t size2, void* fh)
{
    struct MSS_OggFile* of = (struct MSS_OggFile*)(fh);
    size_t len = size1 * size2;
    if ( of->curPtr + len > of->filePtr + of->fileSize )
    {
        len = of->filePtr + of->fileSize - of->curPtr;
    }
    memcpy( dst, of->curPtr, len );
    of->curPtr += len;
    return len;
}

long MSS_tellOgg( void *fh )
{
    struct MSS_OggFile* of = (struct MSS_OggFile*)(fh);
    return (of->curPtr - of->filePtr);
}

int MSS_seekOgg( void *fh, ogg_int64_t to, int type ) 
{
    struct MSS_OggFile* of = (struct MSS_OggFile*)(fh);
	
    switch( type ) 
	{
        case SEEK_CUR:
            of->curPtr += to;
            break;
        case SEEK_END:
            of->curPtr = of->filePtr + of->fileSize - to;
            break;
        case SEEK_SET:
            of->curPtr = of->filePtr + to;
            break;
        default:
            return -1;
    }
	
    if ( of->curPtr < of->filePtr ) 
	{
        of->curPtr = of->filePtr;
        return -1;
    }
    if ( of->curPtr > of->filePtr + of->fileSize ) 
	{
        of->curPtr = of->filePtr + of->fileSize;
        return -1;
    }
    return 0;
}

int MSS_closeOgg(void* fh)
{
    struct MSS_OggFile* of = (struct MSS_OggFile*)(fh);
    delete of;
    return 0;
}

