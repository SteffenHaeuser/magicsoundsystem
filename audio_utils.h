#ifndef MAGICSOUND_AUDIOUTILS_H
#define MAGICSOUND_AUDIOUTILS_H

long MSS_LongSwap (char* buffer);
short MSS_ShortSwap (char* buffer);
bool MSS_AnalyzeAIFFHeader(const char *filename, int *freq, int *channels, int *bitsPerSample, unsigned char **audioBuffer, unsigned int *audioLength);
bool MSS_AnalyzeWAVHeader(unsigned char* buffer, unsigned int size, int* freq, int* channels, int* bitsPerSample);
bool MSS_LoadAudioFromBuffer(unsigned char* buffer, unsigned int size, unsigned char** audioBuffer, unsigned int* audioLength);
size_t MSS_readOgg(void* dst, size_t size1, size_t size2, void* fh);
long MSS_tellOgg( void *fh );
int MSS_seekOgg( void *fh, ogg_int64_t to, int type );
int MSS_closeOgg(void* fh);

#endif