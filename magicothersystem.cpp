#include <SDL/SDL.h>
#include <sys/time.h>

struct MssEvent
{
	int type;
	int key;
	int state;
};

extern "C" unsigned long MSS_GetTicks()
{
#ifdef __PPC__
	return SDL_GetTicks();
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return (Uint32)((time.tv_sec * 1000 + time.tv_usec / 1000)/1);
#endif
}

extern "C" void *MSS_AddTimer(unsigned int interval, Uint32 (*callback)(Uint32, void *), void *param)
{
    return SDL_AddTimer(interval, (SDL_NewTimerCallback)callback, param);
}

extern "C" void MSS_RemoveTimer(void *timerid)
{
	if (timerid==0) return;
	SDL_RemoveTimer((SDL_TimerID)timerid);
}

extern "C" void MSS_CloseScreen(void *screen)
{
	SDL_FreeSurface((SDL_Surface*)screen);
}

extern "C" void *MSS_OpenScreen(int width, int height, int depth, int fullscreen, char *title)
{
	void *screen;
	char *icon;
	char *title2;
	
	screen = SDL_CreateRGBSurface(0, width, height, depth, 0, 0, 0, 0);
	
	if (!screen) return 0;
	
	if (fullscreen)
	{
		screen = SDL_SetVideoMode(width, height, depth, SDL_FULLSCREEN | SDL_HWPALETTE);
	}
	else
	{
		screen = SDL_SetVideoMode(width, height, depth, SDL_HWPALETTE);		
	}
	
	if (!screen) return 0;
	
	SDL_WM_GetCaption(&title2, &icon);
    SDL_WM_SetCaption(title, icon);	
	
	return screen;
}

extern "C" void MSS_Flip(void *screen)
{
	//SDL_BlitSurface((SDL_Surface*)screen, NULL, (SDL_Surface*)screen, NULL);
	SDL_Flip((SDL_Surface*)screen);		
}

extern "C" void MSS_SetColors(void *screen, int startCol, int skipCols, int numCols, unsigned char *rvalues, unsigned char *gvalues, unsigned char *bvalues)
{
	int i;
	SDL_Color  dst_col[256];
	
	for (i = startCol; i < numCols; i++)
    {
		dst_col[i].r = rvalues[i];
		dst_col[i].g = gvalues[i];
		dst_col[i].b = bvalues[i];
	}
	
	SDL_SetColors((SDL_Surface*)screen, dst_col, startCol + skipCols, numCols);	
}

extern "C" int MSS_GetMouseState(int *x, int *y)
{
	int m_x;
	int m_y;
	int m_flags;
	int flags;
	
	m_flags = SDL_GetMouseState(&m_x, &m_y);
	flags = 0;
	
	*x = m_x;
	*y = m_y;
	
	if (m_flags & SDL_BUTTON(1)) flags |= 1;
	if (m_flags & SDL_BUTTON(2)) flags |= 2;
	if (m_flags & SDL_BUTTON(3)) flags |= 4;
	
	return flags;
}

extern "C" int MSS_PeepKeyDownEvent(struct MssEvent *event)
{
	SDL_Event	evt;
	int res;
	
	res = SDL_PeepEvents(&evt, 1, SDL_PEEKEVENT, SDL_KEYDOWNMASK);
	if (res<=0) return 0;
	event->type = 0;
	event->key = evt.key.keysym.sym;
	event->state = evt.key.keysym.mod;
	
	return 1;
}

extern "C" void MSS_PumpEvents()
{
	SDL_PumpEvents();
}

extern "C" int MSS_PollEvent(struct MssEvent *event)
{
	SDL_Event evt;
	
	if (!SDL_PollEvent(&evt))
	{
		return 0;
	}
	switch(evt.type)
	{
		case SDL_KEYDOWN:
			event->type = 1;
			event->key = evt.key.keysym.sym;
			event->state = evt.key.keysym.mod;
			break;
		case SDL_QUIT:
			event->type = 2;
			break;
		case SDL_MOUSEBUTTONDOWN:
			event->type = 4;
			break;
		default:
			event->type = 0;
			break;
	}
	
	return 1;
}

extern "C" void MSS_FillRect(void *screen, int col, int x, int y, int width, int height)
{
    SDL_Rect r;

	r.x = x;
	r.y = y;
    r.w       = width;
    r.h       = height;
    SDL_FillRect((SDL_Surface*)screen, &r, col); 	
}

extern "C" void MSS_ShowCursor(int enable)
{
	if (enable) SDL_ShowCursor(SDL_ENABLE);
	else SDL_ShowCursor(SDL_DISABLE);
}

extern "C" void MSS_LockScreen(void *screen)
{
	SDL_LockSurface((SDL_Surface*)screen);
}

extern "C" void MSS_UnlockScreen(void *screen)
{
	SDL_UnlockSurface((SDL_Surface*)screen);
}
