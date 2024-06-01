#define USENODLL

#include <stdlib.h>

#include "dll.h"

#define bool int

typedef int (*MSS_OpenWaveFunc)(const char*, unsigned char**, unsigned int*, int);
typedef void (*MSS_CloseWaveFunc)(void *, void *);

void *MSS_LoadStreamFromMemory(void *mem_ptr, int len, int stream_type);
void *MSS_LoadSample(const char* name);
void *MSS_LoadAudioFromMemory(const char* audioBuffer, unsigned int audioLength, int channels, int freq, int bitsPerSample);

// Global Functions

void MSS_SetAudioSystemLoaders(MSS_OpenWaveFunc loadWav, MSS_CloseWaveFunc freeWav); 			   
int MSS_SoundInit(int freuency);			
void MSS_SoundClose();			
void MSS_SoundOnOff(int on);
double MSS_GetMusicVolume();
void MSS_SetMusicVolume(double mvol);
int MSS_GetSoundOnOff();
void MSS_SetWAVDirectory(const char* dirname1, const char* dirname2);
void MSS_SetStreamThreshold(int threshold);

// Functions individual per Sound Source

void MSS_SetVolume(void *handle, double vol);
void MSS_SetPan(void *handle, double pan);
int MSS_IsPlaying(void *handle);
void MSS_Play(void *handle, double _vol, double _pan, int looped, bool ignoreGlobalVolume);
void MSS_Stop(void *handle);
void MSS_Free(void *handle);
void MSS_ConvertAudioFormat(void *handle);  
void MSS_SetLog(void *handle, FILE *thelog);
float MSS_GetPlaytime(void *handle);
float MSS_GetCurrenttime(void *handle);
int MSS_GetLooped(void *handle);
void MSS_SetLooped(void *handle, bool looped);

// Utility functions

void* MSS_LoadStreamToSoundFX(const char *filename);
int MSS_OpenWave(const char* name, unsigned char** vbuffer, unsigned int* size, int UsedInWindowsOnly);
void MSS_CloseWave(void* audioBuffer, void *fileBuffer);

void*  dllFindResource(int id, char *pType)
{
    return NULL;
}

void*  dllLoadResource(void *pHandle)
{
    return NULL;
}

void  dllFreeResource(void *pHandle)
{
    return;
}

dll_tExportSymbol DLL_ExportSymbols[]=
{
    {dllFindResource,"dllFindResource"},
    {dllLoadResource,"dllLoadResource"},
    {dllFreeResource,"dllFreeResource"},
	{MSS_LoadStreamFromMemory,"MSS_LoadStreamFromMemory"},
	{MSS_LoadSample,"MSS_LoadSample"},
	{MSS_SetAudioSystemLoaders,"MSS_SetAudioSystemLoaders"},
	{MSS_SoundInit,"MSS_SoundInit"},
	{MSS_SoundClose,"MSS_SoundClose"},
	{MSS_SoundOnOff,"MSS_SoundOnOff"},
	{MSS_GetMusicVolume,"MSS_GetMusicVolume"},
	{MSS_SetMusicVolume,"MSS_SetMusicVolume"},
	{MSS_SetVolume,"MSS_SetVolume"},
	{MSS_SetPan,"MSS_SetPan"},
	{MSS_IsPlaying,"MSS_IsPlaying"},
	{MSS_Play,"MSS_Play"},
	{MSS_Stop,"MSS_Stop"},
	{MSS_Free,"MSS_Free"},
	{MSS_ConvertAudioFormat,"MSS_ConvertAudioFormat"},
	{MSS_SetLog,"MSS_SetLog"},
	{MSS_GetPlaytime,"MSS_GetPlaytime"},
	{MSS_GetCurrenttime,"MSS_GetCurrenttime"},
	{MSS_GetLooped,"MSS_GetLooped"},
	{MSS_GetSoundOnOff,"MSS_GetSoundOnOff"},	
	{MSS_LoadStreamToSoundFX,"MSS_LoadStreamToSoundFX"},
	{MSS_LoadAudioFromMemory,"MSS_LoadAudioFromMemory"},
	{MSS_SetWAVDirectory,"MSS_SetWAVDirectory"},
	{MSS_SetLooped,"MSS_SetLooped"},
	{MSS_SetStreamThreshold,"MSS_SetStreamThreshold"},
    {0,0}
};

dll_tImportSymbol DLL_ImportSymbols[]=
{
    {0,0,0,0}
};

int  DLL_Init(void)
{
    return 1L;
}

void  DLL_DeInit(void)
{
}