#ifndef MAGIC_SOUNDSYSTEM_H
#define MAGIC_SOUNDSYSTEM_H

#define MSS_TYPE_OGG 0
#define MSS_TYPE_MP3 1
#define MSS_TYPE_AIFF 2

struct MssEvent
{
	int type;
	int key;
	int state;
};

typedef enum 
{
	MSS_BACKSPACE = 8,
	MSS_TAB	= 9,
	MSS_RETURN	= 13,
	MSS_ESCAPE = 27,
	MSS_SPACE = 32,
	MSS_DOLLAR = 36,
	MSS_AMPERSAND = 38,
	MSS_QUOTE = 39,
	MSS_LEFTPARENTHESIS = 40,
	MSS_RIGHTPARENTHESIS = 41,
	MSS_ASTERISK = 42,
	MSS_PLUS = 43,
	MSS_COMMA = 44,
	MSS_MINUS = 45,
	MSS_PERIOD = 46,
	MSS_SLASH = 47,
	MSS_0 = 48,
	MSS_1 = 49,
	MSS_2 = 50,
	MSS_3 = 51,
	MSS_4 = 52,
	MSS_5 = 53,
	MSS_6 = 54,
	MSS_7 = 55,
	MSS_8 = 56,
	MSS_9 = 57,
	MSS_COLON = 58,
	MSS_SEMICOLON = 59,
	MSS_LESS = 60,
	MSS_EQUALS = 61,
	MSS_GREATER = 62,
	MSS_QUESTIONMARK = 63,
	MSS_AT = 64,
	MSS_LEFTBRACKET	= 91,
	MSS_BACKSLASH = 92,
	MSS_RIGHTBRACKET = 93,
	MSS_CARET = 94,
	MSS_UNDERSCORE = 95,
	MSS_BACKQUOTE = 96,
	MSS_a = 97,
	MSS_b = 98,
	MSS_c = 99,
	MSS_d = 100,
	MSS_e = 101,
	MSS_f = 102,
	MSS_g = 103,
	MSS_h = 104,
	MSS_i = 105,
	MSS_j = 106,
	MSS_k = 107,
	MSS_l = 108,
	MSS_m = 109,
	MSS_n = 110,
	MSS_o = 111,
	MSS_p = 112,
	MSS_q = 113,
	MSS_r = 114,
	MSS_s = 115,
	MSS_t = 116,
	MSS_u = 117,
	MSS_v = 118,
	MSS_w = 119,
	MSS_x = 120,
	MSS_y = 121,
	MSS_z = 122,
	MSS_DELETE = 127,
	MSS_KP0	= 256,
	MSS_KP1 = 257,
	MSS_KP2 = 258,
	MSS_KP3	= 259,
	MSS_KP4	= 260,
	MSS_KP5 = 261,
	MSS_KP6	= 262,
	MSS_KP7	= 263,
	MSS_KP8	= 264,
	MSS_KP9	= 265,
	MSS_KP_PERIOD = 266,
	MSS_KP_DIVIDE = 267,
	MSS_KP_MULTIPLY	= 268,
	MSS_KP_MINUS = 269,
	MSS_KP_PLUS	= 270,
	MSS_KP_ENTER = 271,
	MSS_KP_EQUALS = 272,
	MSS_UP = 273,
	MSS_DOWN = 274,
	MSS_RIGHT = 275,
	MSS_LEFT = 276,
	MSS_INSERT = 277,
	MSS_HOME = 278,
	MSS_END = 279,
	MSS_PAGEUP = 280,
	MSS_PAGEDOWN = 281,
	MSS_F1 = 282,
	MSS_F2 = 283,
	MSS_F3 = 284,
	MSS_F4 = 285,
	MSS_F5 = 286,
	MSS_F6 = 287,
	MSS_F7 = 288,
	MSS_F8 = 289,
	MSS_F9 = 290,
	MSS_F10 = 291,
	MSS_CAPSLOCK = 301,
	MSS_RSHIFT = 303,
	MSS_LSHIFT = 304,
	MSS_RCTRL = 305,
	MSS_LCTRL = 306,
	MSS_RALT = 307,
	MSS_LALT = 308,
	MSS_PRINT = 316,
	MSS_BREAK = 318,
} MSS_Keys;

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
extern "C" void (*MSS_SetWAVDirectory)(const char* dirname1, const char* dirname2);
extern "C" void (*MSS_SetStreamThreshold)(int threshold);

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

extern "C" unsigned long (*MSS_GetTicks)();
extern "C" void *(*MSS_AddTimer)(unsigned int interval, unsigned int * (*callback)(unsigned int, void *), void *param);
extern "C" void (*MSS_RemoveTimer)(void *timerid);
extern "C" void (*MSS_Flip)(void *screen);
extern "C" void *(*MSS_OpenScreen)(int width, int height, int depth, int fullscreen, char *title);
extern "C" void (*MSS_CloseScreen)(void *screen);
extern "C" void (*MSS_SetColors)(void *screen, int startCol, int skipCols, int numCols, unsigned char *rvalues, unsigned char *gvalues, unsigned char *bvalues);
extern "C" int (*MSS_GetMouseState)(int *x, int *y);
extern "C" int (*MSS_PeepKeyDownEvent)(struct MssEvent *event);
extern "C" void (*MSS_PumpEvents)();
extern "C" int (*MSS_PollEvent)(struct MssEvent *event);
extern "C" void (*MSS_FillRect)(void *screen, int col, int x, int y, int width, int height);
extern "C" void (*MSS_ShowCursor)(int enable);
extern "C" void (*MSS_LockScreen)(void *screen);
extern "C" void (*MSS_UnlockScreen)(void *screen);
extern "C" void (*MSS_CloseDown)();
extern "C" void *(*MSS_GetWindow)(void *window); 
extern "C" void (*MSS_DrawArray)(void *screen, unsigned char* src, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int srcwidth, unsigned int dstwidth);
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
extern "C" void MSS_SetStreamThreshold(int threshold);

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

// Non-Sound Functions

extern "C" unsigned long MSS_GetTicks();
extern "C" void *MSS_AddTimer(unsigned int interval, unsigned int * (*callback)(unsigned int, void *), void *param);
extern "C" void MSS_RemoveTimer(void *timerid);
extern "C" void MSS_Flip(void *screen);
extern "C" void *MSS_OpenScreen(int width, int height, int depth, int fullscreen, char *title);
extern "C" void MSS_CloseScreen(void *screen);
extern "C" void MSS_SetColors(void *screen, int startCol, int skipCols, int numCols, unsigned char *rvalues, unsigned char *gvalues, unsigned char *bvalues);
extern "C" int MSS_GetMouseState(int *x, int *y);
extern "C" int MSS_PeepKeyDownEvent(struct MssEvent *event);
extern "C" void MSS_PumpEvents();
extern "C" int MSS_PollEvent(struct MssEvent *event);
extern "C" void MSS_FillRect(void *screen, int col, int x, int y, int width, int height);
extern "C" void MSS_ShowCursor(int enable);
extern "C" void MSS_LockScreen(void *screen);
extern "C" void MSS_UnlockScreen(void *screen);
extern "C" void MSS_CloseDown();
extern "C" void* MSS_GetWindow(void *window);
extern "C" void MSS_DrawArray(void *screen, unsigned char* src, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int srcwidth, unsigned int dstwidth);
#endif

// Utility functions

#ifndef USENODLL
extern "C" void* (*MSS_LoadStreamToSoundFX)(const char *filename);
#else
extern "C" void* MSS_LoadStreamToSoundFX(const char *filename);
#endif
#endif