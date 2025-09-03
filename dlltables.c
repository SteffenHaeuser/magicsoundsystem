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

// Non-Sound functions

unsigned long MSS_GetTicks();
void *MSS_AddTimer(unsigned int interval, unsigned int * (*callback)(unsigned int, void *), void *param);
void MSS_RemoveTimer(void *timerid);
void MSS_Flip(void *screen);
void *MSS_OpenScreen(int width, int height, int depth, int fullscreen, char *title);
void MSS_CloseScreen(void *screen);
void MSS_SetColors(void *screen, int startCol, int skipCols, int numCols, unsigned char *rvalues, unsigned char *gvalues, unsigned char *bvalues);
int MSS_GetMouseState(int *x, int *y);
int MSS_PeepKeyDownEvent(struct MssEvent *event);
void MSS_PumpEvents();
int MSS_PollEvent(struct MssEvent *event);
void MSS_FillRect(void *screen, int col, int x, int y, int width, int height);
void MSS_ShowCursor(int enable);
void MSS_LockScreen(void *screen);
void MSS_UnlockScreen(void *screen);
void MSS_CloseDown();
void* MSS_GetWindow(void *window) ;
void MSS_DrawArray(void *screen, unsigned char* src, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int srcwidth, unsigned int dstwidth);

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
	{MSS_AddTimer,"MSS_AddTimer"},
	{MSS_RemoveTimer,"MSS_RemoveTimer"},
	{MSS_GetTicks,"MSS_GetTicks"},
	{MSS_OpenScreen,"MSS_OpenScreen"},
	{MSS_CloseScreen,"MSS_CloseScreen"},
	{MSS_Flip,"MSS_Flip"},
	{MSS_SetColors,"MSS_SetColors"},
	{MSS_GetMouseState,"MSS_GetMouseState"},
	{MSS_PeepKeyDownEvent,"MSS_PeepKeyDownEvent"},
	{MSS_PumpEvents,"MSS_PumpEvents"},
	{MSS_PollEvent,"MSS_PollEvent"},
	{MSS_FillRect,"MSS_FillRect"},
	{MSS_ShowCursor,"MSS_ShowCursor"},
	{MSS_LockScreen,"MSS_LockScreen"},
	{MSS_UnlockScreen,"MSS_UnlockScreen"},
	{MSS_CloseDown,"MSS_CloseDown"},
	{MSS_GetWindow,"MSS_GetWindow"},
	{MSS_DrawArray,"MSS_DrawArray"},
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