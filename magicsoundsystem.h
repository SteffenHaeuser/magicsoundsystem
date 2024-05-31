#ifndef MAGIC_SOUNDSYSTEM_H
#define MAGIC_SOUNDSYSTEM_H

#define MSS_TYPE_OGG 0
#define MSS_TYPE_MP3 1
#define MSS_TYPE_AIFF 2

extern "C" typedef int (*MSS_OpenWaveFunc)(const char*, unsigned char**, unsigned int*, int);
extern "C" typedef void (*MSS_CloseWaveFunc)(void *, void *);
#ifndef ONLYCVERSION
extern "C" typedef int (*MSS_OpenWaveAmigaFunc)(const char*, void*&, int&);
#endif
extern "C" MSS_OpenWaveFunc MSS_OpenWave_fct;
extern "C" MSS_CloseWaveFunc MSS_CloseWave_fct;
#ifndef ONLYCVERSION
extern "C" MSS_OpenWaveAmigaFunc MSS_OpenWaveAmiga_fct;
#endif
extern bool MSS_UseNewLoader;

// Functions to create a sound source


#ifndef USENODLL
extern "C" void *(*MSS_LoadStreamFromMemory)(void *mem_ptr, int len, int stream_type);
extern "C" void *(*MSS_LoadSample)(const char* name);
extern "C" void *(*MSS_LoadAudioFromMemory)(const char* audioBuffer, unsigned int audioLength, int channels, int freq, int bitsPerSample);

// Global Functions

extern "C" void (*MSS_SetAudioSystemLoaders)(MSS_OpenWaveFunc loadWav, MSS_CloseWaveFunc freeWav); 	
#ifndef ONLYCVERSION
extern "C" void (*MSS_SetAudioSystemLoaders2)(MSS_OpenWaveAmigaFunc loadWav, MSS_CloseWaveFunc freeWav); 
#endif						   
extern "C" int (*MSS_SoundInit)(int frequency);			
extern "C" void (*MSS_SoundClose)();			
extern "C" void (*MSS_SoundOnOff)(int on);
extern "C" double (*MSS_GetMusicVolume)();
extern "C" void (*MSS_SetMusicVolume)(double mvol);
extern "C" int (*MSS_GetSoundOnOff)();
extern "C" void (*MSS_SetWAVDirectory)(const char* dirname1, const char* dirname2)

// Functions individual per Sound Source

extern "C" void (*MSS_SetVolume)(void *handle, double vol);
extern "C" void (*MSS_SetPan)(void *handle, double pan);
extern "C" int (*MSS_IsPlaying)(void *handle);
extern "C" void (*MSS_Play)(void *handle, double _vol, double _pan, int looped, bool ignoreGlobalVolume);
extern "C" void (*MSS_Stop)(void *handle);
extern "C" void (*MSS_Free)(void *handle);
extern "C" void (*MSS_ConvertAudioFormat)(void *handle);  
extern "C" void (*MSS_SetLog)(void *handle, FILE *thelog);
extern "C" float (*MSS_GetPlaytime)(void *handle);
extern "C" float (*MSS_GetCurrenttime)(void *handle);
extern "C" int (*MSS_GetLooped)(void *handle);
extern "C" void (*MSS_SetLooped)(void *handle, bool looped);
#else
extern "C" void *MSS_LoadStreamFromMemory(void *mem_ptr, int len, int stream_type);
extern "C" void *MSS_LoadSample(const char* name);
extern "C" void *MSS_LoadAudioFromMemory(const char* audioBuffer, unsigned int audioLength, int channels, int freq, int bitsPerSample);

// Global Functions

extern "C" void MSS_SetAudioSystemLoaders(MSS_OpenWaveFunc loadWav, MSS_CloseWaveFunc freeWav); 	
#ifndef ONLYCVERSION
extern "C" void MSS_SetAudioSystemLoaders2(MSS_OpenWaveAmigaFunc loadWav, MSS_CloseWaveFunc freeWav); 
#endif						   
extern "C" int MSS_SoundInit(int freuency);			
extern "C" void MSS_SoundClose();			
extern "C" void MSS_SoundOnOff(int on);
extern "C" double MSS_GetMusicVolume();
extern "C" void MSS_SetMusicVolume(double mvol);
extern "C" int MSS_GetSoundOnOff();
extern "C" void MSS_SetWAVDirectory(const char* dirname1, const char* dirname2);

// Functions individual per Sound Source

extern "C" void MSS_SetVolume(void *handle, double vol);
extern "C" void MSS_SetPan(void *handle, double pan);
extern "C" int MSS_IsPlaying(void *handle);
extern "C" void MSS_Play(void *handle, double _vol, double _pan, int looped, bool ignoreGlobalVolume);
extern "C" void MSS_Stop(void *handle);
extern "C" void MSS_Free(void *handle);
extern "C" void MSS_ConvertAudioFormat(void *handle);  
extern "C" void MSS_SetLog(void *handle, FILE *thelog);
extern "C" float MSS_GetPlaytime(void *handle);
extern "C" float MSS_GetCurrenttime(void *handle);
extern "C" int MSS_GetLooped(void *handle);
extern "C" void MSS_SetLooped(void *handle, bool looped);
#endif

// Utility functions

#ifndef USENODLL
extern "C" void* (*MSS_LoadStreamToSoundFX)(const char *filename);
#else
extern "C" void* MSS_LoadStreamToSoundFX(const char *filename);
#endif
#endif