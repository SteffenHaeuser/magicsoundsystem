#include <sdl/SDL.h>
#include <sys/time.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <exec/types.h>        // Basic types and definitions
#include <exec/memory.h>      // Memory allocation and deallocation functions
#include <exec/libraries.h>   // Library management functions
#include <graphics/gfx.h>     // Graphics structures and functions
#include <graphics/rastport.h> // RastPort structure and graphics operations
#include <intuition/intuition.h> // Intuition structures and functions (window management)
#include <intuition/icclass.h> // Intuition IDCMP classes (event classes)
#include <workbench/startup.h> // Workbench startup structures (optional, depending on your needs)
#include <SDL/SDL.h>

#include <exec/types.h>
#include <intuition/screens.h>
#include <graphics/view.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <proto/lowlevel.h>
#include <libraries/lowlevel.h>

struct Screen *wbScreen;
ULONG *originalColors = NULL;  // Array to hold original colors
ULONG numColors;

void BackupColors(struct Screen *wbScreen)
{
    if (wbScreen == NULL) return;

    // Get the ColorMap from the ViewPort
    struct ColorMap *cm = wbScreen->ViewPort.ColorMap;
    if (cm == NULL) return;

    // Allocate memory for originalColors: 1 for count/start, 3*numColors for RGB, and 1 for terminator
    originalColors = (ULONG*)AllocVec((2 + (3 * 256)) * sizeof(ULONG), MEMF_CLEAR|MEMF_CHIP);
    
    if (originalColors == NULL) return;

    // Set the first word (high 16 bits = numColors, low 16 bits = starting index)
    originalColors[0] = (256 << 16) | 0;  // Assuming starting index is 0

    // Create a temporary table to hold the color data
    ULONG *colorTable = (ULONG*)AllocVec(3 * 256 * sizeof(ULONG), MEMF_CHIP|MEMF_CLEAR);
    if (colorTable == NULL) {
        FreeVec(originalColors);
        originalColors = NULL;
        return;
    }

    // Retrieve the RGB values using GetRGB32
    GetRGB32(cm, 0, 256, colorTable);

    // Copy the RGB values into the originalColors array
    for (int i = 0; i < 3 * 256; i++)
    {
        originalColors[1 + i] = colorTable[i];
    }

    // Free the temporary table
    FreeVec(colorTable);

    // Set the terminating zero
    originalColors[1 + (3 * 256)] = 0;
}


// Restore original colors
void RestoreColors(struct Screen *wbScreen)
{
    if (wbScreen != NULL && originalColors != NULL)
    {
        // Restore the original colors
        LoadRGB32(&(wbScreen->ViewPort), originalColors);
    }

    // Free allocated memory
    if (originalColors != NULL)
    {
        FreeVec(originalColors);
        originalColors = NULL;
    }
}


int AmigaToSDLKey(int amigaKeyCode) {
		if (amigaKeyCode>128) amigaKeyCode=amigaKeyCode-128;
    switch (amigaKeyCode) {
        case 27: return SDLK_ESCAPE;         // kbESC
        case 49: return SDLK_1;             // kb1
        case 50: return SDLK_2;             // kb2
        case 51: return SDLK_3;             // kb3
        case 52: return SDLK_4;             // kb4
        case 53: return SDLK_5;             // kb5
        case 54: return SDLK_6;             // kb6
        case 55: return SDLK_7;             // kb7
        case 56: return SDLK_8;             // kb8
        case 57: return SDLK_9;             // kb9
        case 48: return SDLK_0;             // kb0
        case 97: return SDLK_a;             // kbA
        case 98: return SDLK_b;             // kbB
        case 99: return SDLK_c;             // kbC
        case 100: return SDLK_d;             // kbD
        case 101: return SDLK_e;             // kbE
        case 102: return SDLK_f;             // kbF
        case 103: return SDLK_g;             // kbG
        case 104: return SDLK_h;             // kbH
        case 105: return SDLK_i;             // kbI
        case 106: return SDLK_j;             // kbJ
        case 107: return SDLK_k;             // kbK
        case 108: return SDLK_l;             // kbL
        case 109: return SDLK_m;             // kbM
        case 110: return SDLK_n;             // kbN
        case 111: return SDLK_o;             // kbO
        case 112: return SDLK_p;             // kbP
        case 113: return SDLK_q;             // kbQ
        case 114: return SDLK_r;             // kbR
        case 115: return SDLK_s;             // kbS
        case 116: return SDLK_t;             // kbT
        case 117: return SDLK_u;             // kbU
        case 118: return SDLK_v;             // kbV
        case 119: return SDLK_w;             // kbW
        case 120: return SDLK_x;             // kbX
        case 121: return SDLK_y;             // kbY
        case 122: return SDLK_z;             // kbZ
        case 32: return SDLK_SPACE;         // kbSPACE
        case 13: return SDLK_RETURN;        // kbRETURN
		case 284: return SDLK_LCTRL;			// kbCTRL
        case 286: return SDLK_LSHIFT;        // kbLSHIFT
        case 305: return SDLK_RSHIFT;        // kbRSHIFT		
        case 289: return SDLK_LALT;          // kbALT
        case 290: return SDLK_RALT;          // kbALTGR (considered same as kbALT for this mapping)		
        default: return -1;          // Unknown key
    }
}

int keyConvert2(int amigaKeyCode) {
	if (amigaKeyCode>128) amigaKeyCode=amigaKeyCode-128;
    switch (amigaKeyCode) {
        case 208-128: return SDLK_F1;            // kbF1
        case 209-128: return SDLK_F2;            // kbF2
        case 210-128: return SDLK_F3;            // kbF3
        case 211-128: return SDLK_F4;            // kbF4
        case 212-128: return SDLK_F5;            // kbF5
        case 213-128: return SDLK_F6;            // kbF6
        case 214-128: return SDLK_F7;            // kbF7
        case 215-128: return SDLK_F8;            // kbF8
        case 216-128: return SDLK_F9;            // kbF9
        case 217-128: return SDLK_F10;           // kbF10
        case 103: return SDLK_HOME;          // kbHOME
        case 76: return SDLK_UP;            // kbCRSRUP
        case 63: return SDLK_PAGEUP;        // kbPAGEUP
        case 79: return SDLK_LEFT;          // kbCRSRLEFT
        case 78: return SDLK_RIGHT;         // kbCRSRRIGHT
        case 77: return SDLK_DOWN;          // kbCRSRDOWN
        case 31: return SDLK_PAGEDOWN;      // kbPAGEDOWN
        case 102: return SDLK_INSERT;        // kbINSERT
        case 70: return SDLK_DELETE;        // kbDELETE
        case 65: return SDLK_BACKSPACE;     // kbBACKSPACE
        case 56: return SDLK_COMMA;         // kbKOMMA
        case 57: return SDLK_PERIOD;        // kbPOINT
        case 58: return SDLK_MINUS;         // kbMINUS
        case 66: return SDLK_TAB;           // kbTAB
        case 27: return SDLK_PLUS;          // kbPLUS
		case 29: return SDLK_END;			// kbEnd
        case 61: return SDLK_HOME;          // kbNUM7
        case 74: return SDLK_KP_MINUS;      // kbNUMMINUS
        case 92: return SDLK_KP_DIVIDE;     // kbNUMDIVIDE
        default: return SDLK_UNKNOWN;          // Unknown key
    }
}

struct MssEvent
{
	int type;
	int key;
	int state;
};
#define MAX_EVENTS 256

struct MssEvent eventQueue[MAX_EVENTS];
int eventQueueStart = 0, eventQueueEnd = 0;

int isEventQueueFull() 
{
    return (eventQueueEnd + 1) % MAX_EVENTS == eventQueueStart;
}

int isEventQueueEmpty() 
{
    return eventQueueStart == eventQueueEnd;
}

void pushEventToQueue(struct MssEvent event) 
{	
    if (!isEventQueueFull()) 
	{
        eventQueue[eventQueueEnd].type = event.type;
        eventQueue[eventQueueEnd].key = event.key;
        eventQueue[eventQueueEnd].state = event.state;		
        eventQueueEnd = (eventQueueEnd + 1) % MAX_EVENTS;
    } else 
	{
		eventQueueStart = (eventQueueStart + 1) % MAX_EVENTS;
    }
}

int popEventFromQueue(struct MssEvent *event) 
{
    if (!isEventQueueEmpty()) 
	{
        event->type = eventQueue[eventQueueStart].type;
	    event->key = eventQueue[eventQueueStart].key;
        event->state = eventQueue[eventQueueStart].state;		
        eventQueueStart = (eventQueueStart + 1) % MAX_EVENTS;
        return 1;
    }
    return 0;
}

struct MssAmigaScreen {
    struct Screen *screen;  // Pointer to the screen (custom or Workbench)
    struct Window *window;  // Pointer to the window (only used in non-fullscreen mode)
    int fullscreen;         // Flag to track if fullscreen mode is active (1 = fullscreen, 0 = windowed)
	ULONG videoMemoryAddress; // Address of first Bitplane
	UWORD *originalPointerShape;
	int width;
	int height;
	struct BitMap *bitMap;  // Common BitMap for both buffers
    struct ScreenBuffer *screenBuffers[2];  // Two buffers for double buffering
    struct RastPort rastPort;  // RastPort for drawing
    BOOL bufferToggle;         // Used to track which buffer is active	
	void *pointer;
#ifdef USEGRAFFITI
    UBYTE *framebuffer;
    ULONG bytesPerRow;
#endif
};

struct MssAmigaScreen *mssAmigaScreen = 0;

extern "C" unsigned long MSS_GetTicks()
{
#ifdef __amigaos4__
	return SDL_GetTicks();
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return (Uint32)((time.tv_sec * 1000 + time.tv_usec / 1000)/1);
#endif
}

extern "C" void *MSS_AddTimer(unsigned int interval, Uint32 (*callback)(Uint32, void *), void *param)
{
	return 0;
}

extern "C" void MSS_RemoveTimer(void *timerid)
{
}

extern "C" void MSS_CloseScreen(void *screenHandle)
{
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen *)screenHandle;
    if (!amigaScreen) return;

    if (amigaScreen->fullscreen)
    {
        if (amigaScreen->window)
        {
            CloseWindow(amigaScreen->window);
        }		
		// Fullscreen: Close the custom screen
        if (amigaScreen->screen)
        {
            CloseScreen(amigaScreen->screen);
        }
    }
    else
    {
        // Non-Fullscreen: Close the window and unlock the Workbench screen
        if (amigaScreen->window)
        {
            CloseWindow(amigaScreen->window);
        }
        if (amigaScreen->screen)
        {
			RestoreColors(amigaScreen->screen);			
            UnlockPubScreen(NULL, amigaScreen->screen);
        }
    }
	
	if (amigaScreen->pointer) FreeVec(amigaScreen->pointer);
	amigaScreen->pointer = 0;

    // Free the structure
    free(amigaScreen);
	mssAmigaScreen = 0;
	if (LowLevelBase)
	{
		CloseLibrary(LowLevelBase);
		LowLevelBase = 0;
	}
}

#ifndef USEAGA
#include <proto/cybergraphics.h>
#endif
int usingAGA = 0;
int firstTimeConfig = 0;
int usingWCP = 1;

static int ModeFitsWidthExactHeightMin(int storedW, int storedH, int reqW, int reqH)
{
    return (storedW == reqW) && (storedH >= reqH);
}

#include <proto/asl.h>
#include <libraries/asl.h>

static int RequestScreenModeWidthExact(ULONG initialMode,
                                       int reqW, int reqH,
                                       ULONG *outMode, int *outW, int *outH)
{
    struct ScreenModeRequester *smr =
        (struct ScreenModeRequester*)AllocAslRequestTags(ASL_ScreenModeRequest,
            ASLSM_InitialDisplayID, initialMode,

            ASLSM_MinWidth,        reqW,
            ASLSM_MaxWidth,        reqW,   // <- Breite EXAKT

            ASLSM_MinHeight,       reqH,   // <- Höhe MINDESTENS
            // ASLSM_MaxHeight weglassen -> darf größer sein

            ASLSM_MinDepth,        8,
            ASLSM_MaxDepth,        8,
            TAG_DONE);

    if (!smr) return 0;

    int ok = AslRequest(smr, NULL);
    if (ok)
    {
        *outMode = smr->sm_DisplayID;
        *outW    = (int)smr->sm_DisplayWidth;
        *outH    = (int)smr->sm_DisplayHeight;
    }

    FreeAslRequest(smr);
    return ok;
}

static int LoadModeFile(const char *path, ULONG *modeId, int *mw, int *mh)
{
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    unsigned long mid = 0;
    int w = 0, h = 0;
    int ok = (fscanf(f, "%lx %d %d", &mid, &w, &h) == 3);
    fclose(f);

    if (!ok) return 0;
    *modeId = (ULONG)mid;
    *mw = w;
    *mh = h;
    return 1;
}

static void SaveModeFile(const char *path, ULONG modeId, int w, int h)
{
    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "0x%08lx %d %d\n", (unsigned long)modeId, w, h);
    fclose(f);
}

static int FileExists(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}


ULONG GetMode(int w, int h)
{
    ULONG modeID, error, result;
    struct DisplayInfo dpinfo;
    struct NameInfo nminfo;
    struct DimensionInfo diminfo;
    DisplayInfoHandle displayhandle;
	
#ifndef USEAGA	
	usingAGA = 0;
#else
	usingAGA = 1;
#endif

if (FileExists("env:Settlers2/UseReq"))
{
    const int isMenu = (w == 640);  // “Menu” Slot per Breite erkennbar
    const char *slotPath = isMenu ? "env:Settlers2/ModeMenu"
                                  : "env:Settlers2/ModeGame";

    ULONG storedMode = INVALID_ID;
    int storedW = 0, storedH = 0;

    int haveStored = LoadModeFile(slotPath, &storedMode, &storedW, &storedH);

    if (haveStored && storedMode != INVALID_ID &&
        ModeFitsWidthExactHeightMin(storedW, storedH, w, h))
    {
        return storedMode;
    }

    // Requester solange wiederholen, bis Breite exakt und Höhe >= reqH oder Abbruch
    ULONG pickedMode = haveStored ? storedMode : INVALID_ID;
    int pickedW = 0, pickedH = 0;

    for (;;)
    {
        if (!RequestScreenModeWidthExact(pickedMode, w, h, &pickedMode, &pickedW, &pickedH))
        {
            // User cancel -> fallback: dein AGA/RTG Auto-Fallback unten
            break;
        }

        if (ModeFitsWidthExactHeightMin(pickedW, pickedH, w, h))
        {
            SaveModeFile(slotPath, pickedMode, pickedW, pickedH);
            return pickedMode;
        }

        // sollte wegen MaxWidth=reqW praktisch nie passieren,
        // aber zur Sicherheit:
    }
}
		
	if (usingAGA)
	{
	if (w==320) 
	{
		int bestMode=0x21000;

		return 0x21000;
	}
	else 
	{
		int bestMode;
		FILE *fil2 = fopen("devs:Monitors/Multiscan","r");
		if (!fil2) bestMode = 0x29004;
		else
		{
			fclose(fil2);
			bestMode=0x39024;
		}		
		return bestMode;
	}
	}
	usingWCP = 0;

    ULONG bestMode = INVALID_ID, bestW=0xffffffff, bestH = 0xffffffff;
    ULONG nW, nH;


    modeID = INVALID_ID;
    
    GetDisplayInfoData(NULL, (UBYTE *)&dpinfo, sizeof(struct DisplayInfo),
	    DTAG_DISP, LORES_KEY);

    while ((modeID = NextDisplayInfo(modeID)) != INVALID_ID)
    {
#if !defined(USEAGA) && !defined(USEGRAFFITI)
	if (CyberGfxBase!=0)
	{
			if (!IsCyberModeID(modeID)) continue;
	}
#endif	
	error = ModeNotAvailable(modeID);
	    
	if (error == 0)
	{
	    displayhandle = FindDisplayInfo(modeID);
	    if (displayhandle)
	    {
		result = GetDisplayInfoData(displayhandle, (UBYTE *)&diminfo,
			    sizeof(struct DimensionInfo), DTAG_DIMS, NULL);
		if (result)
		{
		    if (diminfo.MaxDepth == 8)
		    {
			nW = diminfo.Nominal.MaxX - diminfo.Nominal.MinX + 1;
			nH = diminfo.Nominal.MaxY - diminfo.Nominal.MinY + 1;

			if (nW == h && nH == w)
			{
			    // Exact match
			    return modeID;
			}
			else
			{
			    // Check if the widht and height are big enough to even take the screen
			    if (nW >= w && nH >= h)
			    {
				// Yes. Check if they match the requested size better
				if (nW <= bestW && nH <= bestH)
				{
				    // They do
				    bestW = nW;
				    bestH = nH;
				    bestMode = modeID;
				}
			    }
			}
		    } // if (diminfo...
		} // if (result...
	    } // if (displayhandle...
	} // if (error...
    } // while
	
    // Here, we either have a mode that roughly matches, or none
    return bestMode;
}
extern "C" void *MSS_OpenScreen(int width, int height, int depth, int fullscreen, char *title)
{
	int mode = GetMode(width,height);
	FILE *fil;
	
	if (!LowLevelBase) LowLevelBase = OpenLibrary("lowlevel.library",0);
	
    // Allocate memory for the MssAmigaScreen structure
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen *)malloc(sizeof(struct MssAmigaScreen));
    if (!amigaScreen) return NULL;  // Error allocating memory

    amigaScreen->fullscreen = fullscreen;
    amigaScreen->screen = NULL;
    amigaScreen->window = NULL;
	amigaScreen->width = width;
	amigaScreen->height = height;

	usingWCP = 0;
	fil = fopen("env:Settlers2/UseWCP","r");
	if (fil)
	{
		fclose(fil);
		usingWCP = 1;
	}	
	if (firstTimeConfig==0)
	{
		if (usingAGA) 
		{
			//fprintf(stderr,"Running Settlers 2 in AGA Mode\n");
			//if (usingWCP) fprintf(stderr,"Using WriteChunkyPixels for ScreenRefresh\n");
		//	else fprintf(stderr,"Using c2p for ScreenRefresh\n");
		}
		else 
		{
		//	fprintf(stderr,"Running Settlers2 in Graphics Board Mode\n");
			usingWCP = 1;
		}
	}
	firstTimeConfig = 1;	

	//fullscreen = 0;

    if (fullscreen)
    {
        // Fullscreen mode: Open a custom screen
        struct TagItem screenTags[] = {
            { SA_Left, 0 },
            { SA_Top, 0 },
            { SA_Width, (ULONG)width },
            { SA_Height, (ULONG)height },
            { SA_Depth, (ULONG)depth },
            { SA_Type, CUSTOMSCREEN },  // Custom screen for fullscreen
            { SA_Title, (ULONG)title }, // Screen title
            { SA_DisplayID, mode }, // AGA Lores Mode (adjust as needed)
#ifdef USEGRAFFITI
    { SA_Quiet, TRUE },
    { SA_ShowTitle, FALSE },
#endif
            { TAG_END }
        };
        amigaScreen->screen = OpenScreenTagList(NULL, screenTags);
        if (!amigaScreen->screen)
        {
            free(amigaScreen);  // Free the allocated structure
            return NULL;
        }
    }
    else
    {
        // Non-Fullscreen: Lock the Workbench screen
        amigaScreen->screen = LockPubScreen(NULL);  // Lock the Workbench screen
        if (!amigaScreen->screen)
        {
            free(amigaScreen);  // Free the allocated structure
            return NULL;
        }
		BackupColors(amigaScreen->screen);
    }

    // Open a window regardless of the fullscreen status
    ULONG screentag = fullscreen ? WA_CustomScreen : WA_PubScreen;
    ULONG titletag = fullscreen ? TAG_IGNORE : WA_Title;

    ULONG wflgs = WFLG_REPORTMOUSE
                | WFLG_NOCAREREFRESH
                | WFLG_ACTIVATE
                | WFLG_RMBTRAP
                | (fullscreen ? 0 : WFLG_CLOSEGADGET | WFLG_DEPTHGADGET | WFLG_DRAGBAR); // Add gadgets only in windowed mode
				
	if (fullscreen) wflgs |= WFLG_BORDERLESS;

    amigaScreen->window = OpenWindowTags(NULL,
        WA_Left, 0,
        WA_Top, 0,
        WA_Flags, wflgs,
        WA_InnerWidth, width,
        WA_InnerHeight, height,
        screentag, (ULONG)amigaScreen->screen,
        titletag, (ULONG)title,
        WA_ScreenTitle, (ULONG)title,
        TAG_DONE);

    if (!amigaScreen->window)
    {
        if (fullscreen) {
            CloseScreen(amigaScreen->screen); // Close custom screen on failure
        } else {
            UnlockPubScreen(NULL, amigaScreen->screen); // Unlock Workbench screen on failure
        }
        free(amigaScreen);  // Free the allocated structure
        return NULL;
    }
	
	if (fullscreen)
	{
		amigaScreen->pointer = AllocVec(24, MEMF_CLEAR|MEMF_CHIP);
		if (!amigaScreen->pointer)
		{
		if (amigaScreen->window) CloseWindow(amigaScreen->window);
        if (fullscreen) {
            CloseScreen(amigaScreen->screen); // Close custom screen on failure
        } else {
            UnlockPubScreen(NULL, amigaScreen->screen); // Unlock Workbench screen on failure
        }
        free(amigaScreen);  // Free the allocated structure
        return NULL;			
		}
	
		//ClearPointer(amigaScreen->window);
		SetPointer(amigaScreen->window, (UWORD*)amigaScreen->pointer, 1, 16, 0, 0);		
	}
	else amigaScreen->pointer = 0;
	
	if (amigaScreen->window->UserPort == 0)
	{
	    ModifyIDCMP(amigaScreen->window, IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW);
	}

	mssAmigaScreen = amigaScreen;
	
	/*amigaScreen->bitMap = AllocBitMap(width, height, depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL);
	if (!amigaScreen->bitMap) 
	{
		// Handle error (cleanup screen, free resources, etc.)
		return NULL;
	}*/

	// Allocate two ScreenBuffer structures
	/*amigaScreen->screenBuffers[0] = AllocScreenBuffer(amigaScreen->screen, amigaScreen->bitMap, SB_SCREEN_BITMAP);
	fprintf(stderr,"A1: %x\n",amigaScreen->screenBuffers[0]);
	amigaScreen->screenBuffers[1] = AllocScreenBuffer(amigaScreen->screen, NULL,0);
	fprintf(stderr,"A2: %x\n", amigaScreen->screenBuffers[1]);
	if (!amigaScreen->screenBuffers[0] || !amigaScreen->screenBuffers[1]) 
	{
		// Handle error, clean up, free resources
		if (amigaScreen->screenBuffers[0]) FreeScreenBuffer(amigaScreen->screen, amigaScreen->screenBuffers[0]);
		if (amigaScreen->screenBuffers[1]) FreeScreenBuffer(amigaScreen->screen, amigaScreen->screenBuffers[1]);
		FreeBitMap(amigaScreen->bitMap);
		return NULL;
	}*/
	// Set up the RastPort to point to the bitmap (initially to the first buffer)
	//InitRastPort(&amigaScreen->rastPort);
	//amigaScreen->rastPort.BitMap = amigaScreen->bitMap;
    // Initialize buffer toggle state
	amigaScreen->bufferToggle = 0;	
	//amigaScreen->rastPort.BitMap = amigaScreen->bitMap;	
	/*if (!ChangeScreenBuffer(amigaScreen->screen, amigaScreen->screenBuffers[1-amigaScreen->bufferToggle]))
	{
		fprintf(stderr,"Failed to switch to Buffer 0\n");
	}*/
	//fprintf(stderr,"Writing to Buffer 1\n");

    // Return the structure as a void pointer
    return (void *)amigaScreen;
}

extern "C" void MSS_Flip(void *screen)
{	    
//WaitTOF();
return;
	MssAmigaScreen* amigaScreen = (MssAmigaScreen*)screen;
	if (screen==0) return;
	// Toggle between the two buffers
    amigaScreen->bufferToggle = !amigaScreen->bufferToggle;
	
	fprintf(stderr,"Flipping to Buffer: %d Writing to Buffer %d\n",amigaScreen->bufferToggle,!amigaScreen->bufferToggle);

    // Swap the buffers using ChangeScreenBuffer
    if (!ChangeScreenBuffer(amigaScreen->screen, amigaScreen->screenBuffers[amigaScreen->bufferToggle])) 
	{
		fprintf(stderr,"ChangeScreenBuffer failed\n");
        // Handle error if the buffer swap fails (optional)
	}
	
// Update RastPort to point to the new back buffer (the one not being shown)
    amigaScreen->rastPort.BitMap = amigaScreen->screenBuffers[!amigaScreen->bufferToggle]->sb_BitMap;
	
}

ULONG colors[2 + 3 * 256];  // 2 for the count/first color and the terminator, 3 longs per color

extern "C" void MSS_SetColors(void *screenHandle, int startCol, int skipCols, int numCols, unsigned char *rvalues, unsigned char *gvalues, unsigned char *bvalues)
{
    // Allocate enough space for the color table
    // Each color requires 3 longs (R, G, B), plus one word at the beginning and the terminator
    
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen *)screenHandle;
    if (!amigaScreen || !amigaScreen->screen) return;

    // Calculate the starting index after skipping colors
    int colIndex = startCol + skipCols;

    // Set the first entry: high 16 bits = number of colors, low 16 bits = starting color index
    colors[0] = ((ULONG)numCols << 16) + (ULONG)colIndex;

    // Fill the array with RGB triplets for each color
    int colorIndex = 1;  // Start filling after the first count/index word
    for (int i = 0; i < numCols; i++)
    {
        colors[colorIndex++] = (ULONG)rvalues[startCol+i] * 0x01010101;  // Left-justified 8-bit red value
        colors[colorIndex++] = (ULONG)gvalues[startCol+i] * 0x01010101;  // Left-justified 8-bit green value
        colors[colorIndex++] = (ULONG)bvalues[startCol+i] * 0x01010101;  // Left-justified 8-bit blue value
    }

    // Terminate the table with a 0 (as per the LoadRGB32 specification)
    colors[colorIndex] = 0;

    // Load the color values into the ViewPort using LoadRGB32
    LoadRGB32(&amigaScreen->screen->ViewPort, colors);	
}

int currentLeftButton = 0;
int currentRightButton = 0;
int leftWentDown = 0;
int rightWentDown = 0;
MssEvent *lastEvent = 0;
int rawKeyConvert(int rawkey)
{
    struct InputEvent ie;
    WORD res;
    char cbuf[20];

    UWORD code = rawkey&0xFF;
    UWORD qual = (rawkey>>8)%0xFFFF;
    if (!KeymapBase) return -1;

    switch(code)
    {
    case 61:
    case 62:
    case 63:
    case 45:
    case 46:
    case 47:
    case 29:
    case 30:
    case 31:
    case 15:
    case 60:
	return -1;
    default:
	ie.ie_Class = IECLASS_RAWKEY;
	ie.ie_SubClass = 0;
	ie.ie_Code  = code;
	ie.ie_Qualifier = qual;
	ie.ie_EventAddress = NULL;

	res = MapRawKey(&ie, cbuf, 20, 0);
	if (res != 1)
	{
	    return -1;
	}
	else
	{
	    return cbuf[0];
	}
    }
}

int key_pressed[256];

double lastPressedTime[80] = {0};  // Array to store the last pressed time for each key (76, 77, 78, 79)

Uint32 getCurrentTimeInMilliSeconds()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (Uint32)((time.tv_sec * 1000 + time.tv_usec / 1000)/1);
}


extern "C" void MSS_PumpEvents()
{
    // Ensure you have access to the MssAmigaScreen structure
    struct MssAmigaScreen *amigaScreen = mssAmigaScreen; // Obtain your amigaScreen reference here
	if (!amigaScreen) return;

    struct IntuiMessage *msg;
    static struct MssEvent newEvent;    
	
    // Check if there are messages in the window's UserPort
    while ((msg = (struct IntuiMessage *)GetMsg(amigaScreen->window->UserPort)))
	{
		switch (msg->Class)
		{
			case IDCMP_MOUSEBUTTONS:
			{
				if (msg->Code==IECODE_LBUTTON) 
				{

					currentLeftButton = 1;
					leftWentDown = 1;
					newEvent.type = 4;  // Mouse button event
                    newEvent.key = 1;   // Left button
                    newEvent.state = currentLeftButton;
                    pushEventToQueue(newEvent);				
				} 
				else if (msg->Code==IECODE_LBUTTON+128)
				{
						currentLeftButton = 0;
						newEvent.type = 4;  // Mouse button event
						newEvent.key = 1;   // Left button
						newEvent.state = currentLeftButton;
						pushEventToQueue(newEvent);									
				}
				else if (msg->Code==IECODE_RBUTTON) 
				{
					currentRightButton = 1;
					rightWentDown = 1;
					newEvent.type = 4;  // Mouse button event
                    newEvent.key = 2;   // Right button
                    newEvent.state = currentRightButton;
                    pushEventToQueue(newEvent);						
				}
				else if (msg->Code==IECODE_RBUTTON+128)
				{
						currentRightButton = 0;
						newEvent.type = 4;  // Mouse button event
						newEvent.key = 2;   // Left button
						newEvent.state = currentRightButton;
						pushEventToQueue(newEvent);								
				}
			}			
			break;
			case IDCMP_RAWKEY:
			{
				int vanillaKey;
				int alreadyPressed;
				double currentTime = getCurrentTimeInMilliSeconds();  // Get the current time in milliseconds


                newEvent.type = 1;  // Key event
				if (currentTime - lastPressedTime[76] > 125) key_pressed[76] = 0;
				if (currentTime - lastPressedTime[77] > 125) key_pressed[77] = 0;
				if (currentTime - lastPressedTime[78] > 125) key_pressed[78] = 0;
				if (currentTime - lastPressedTime[79] > 125) key_pressed[79] = 0;
				if (!((msg->Code&0x80)))
				{
					alreadyPressed = key_pressed[msg->Code];
				}
				else alreadyPressed = 0;
				if ((!alreadyPressed)||((msg->Code&0x80)==1))
				{
					vanillaKey = rawKeyConvert(msg->Code);
					if (vanillaKey!=-1) newEvent.key = AmigaToSDLKey(vanillaKey);  // Convert Amiga keycode to SDL keycode
					else newEvent.key = keyConvert2(msg->Code);
					if (newEvent.key == -1) 
					{
						if (msg->Code&128) newEvent.key = keyConvert2(msg->Code-128);
						else newEvent.key = keyConvert2(msg->Code);
					}
					if (!(msg->Code&0x80)) 
					{
						lastPressedTime[msg->Code] = currentTime; 						
						key_pressed[msg->Code] = 1;
					}
					else key_pressed[msg->Code-128] = 0;
					newEvent.state = (msg->Code & 0x80) ? 0 : 1;  // 0 = key released, 1 = key pressed
					if (newEvent.state)
					{
					pushEventToQueue(newEvent);	
					lastEvent = &newEvent;			
					}
				}
			}
			break;
			break;
			default:
			{
			}
			break;
		}
		ReplyMsg((struct Message *)msg);
	}		
}

SDL_Joystick *joystick = 0;
int joy_x = 160;
int joy_y = 240;

double lastJoyTimeUp = 0;
double lastJoyTimeDown = 0;
double lastJoyTimeLeft = 0;
double lastJoyTimeRight = 0;
int leftPressed = 0;
int rightPressed = 0;

extern "C" int MSS_GetMouseState(int *x, int *y)
{
	int l,r;
	int joychanged = 0;
    static struct MssEvent newEvent;    	
	int mouseButtons;
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen *)mssAmigaScreen; // Assuming mssAmigaScreen is a global pointer to your MssAmigaScreen
    if (!amigaScreen || !amigaScreen->window) return 0;

    // Access mouse coordinates directly from the window structure
    *x = amigaScreen->window->MouseX; // Replace with the actual field name for mouse X
    *y = amigaScreen->window->MouseY; // Replace with the actual field name for mouse Y
	
	if (amigaScreen->fullscreen==0)
	{
		*x = *x - amigaScreen->window->BorderLeft;
		*y = *y - amigaScreen->window->BorderTop;
	}
	
	l = currentLeftButton;
	r = currentRightButton;
	mouseButtons = 0;
if (leftWentDown)  l = 1;
if (rightWentDown) r = 1;

if (l) mouseButtons |= 1;
if (r) mouseButtons |= 4;

// Latches nach dem Auslesen löschen
leftWentDown = 0;
rightWentDown = 0;

	if (LowLevelBase)
	{
		int joyState = 0;
		
		joyState = ReadJoyPort(1);
		double currentTime = getCurrentTimeInMilliSeconds();  // Get the current time in milliseconds

		joychanged = 0;
		if (joyState&JPF_JOY_UP)
		{
				if (currentTime - lastJoyTimeUp > 31) 
				{
					if (joy_y>3) 
					{
						joy_y-=4;
						joychanged = 1;
					}
					lastJoyTimeUp = currentTime;
				}	
		}
		if (joyState&JPF_JOY_DOWN)
		{
			if (currentTime - lastJoyTimeDown > 31)
			{
				if (joy_y<475) 
				{
					joy_y+=4;
					joychanged = 1;
				}
				lastJoyTimeDown = currentTime;
			}
		}
		if (joyState&JPF_JOY_LEFT)
		{
			if (currentTime - lastJoyTimeLeft > 31)
			{
				if (joy_x>3) 
				{
					joy_x-=4;
					joychanged = 1;
				}
				lastJoyTimeLeft = currentTime;
			}
		}
		if (joyState&JPF_JOY_RIGHT)
		{
			if (currentTime - lastJoyTimeRight > 31)
			{
				if (joy_x<635) 
				{
					joychanged = 1;
					joy_x+=4;         
				}
				lastJoyTimeRight = currentTime;
			}
		}
		if ((joyState&JPF_BUTTON_RED)&&(leftPressed==0))
		{
			leftPressed = 1;
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 1;   // Left button
			newEvent.state = leftPressed;
			pushEventToQueue(newEvent);	
		}
		else if (((joyState&JPF_BUTTON_RED)==0)&&(leftPressed==1))
		{
			leftPressed = 0;
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 1;   // Left button
			newEvent.state = leftPressed;
			pushEventToQueue(newEvent);				
		}
		if ((joyState&JPF_BUTTON_BLUE)&&(rightPressed==0))
		{
			rightPressed = 1;
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 2;   // Right button
			newEvent.state = rightPressed;
			pushEventToQueue(newEvent);				
		}
		else if (((joyState&JPF_BUTTON_BLUE)==0)&&(rightPressed==1))
		{	
			rightPressed = 0;
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 2;   // Right button
			newEvent.state = rightPressed;
			pushEventToQueue(newEvent);	
		}	
		if (joychanged)
		{
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 3;   // Right button
			newEvent.state = joy_x;
			pushEventToQueue(newEvent);		
			newEvent.type = 8;  // Second Mouse button event
			newEvent.key = 4;   // Right button
			newEvent.state = joy_y;
			pushEventToQueue(newEvent);				
		}
	}
	
	int res;
	
	if (leftPressed) mouseButtons|=16;
	if (rightPressed) mouseButtons|=32; 
	
	res = mouseButtons|(joychanged*8);

    return res;
}
extern "C" int MSS_PeepKeyDownEvent(struct MssEvent *event)
{
    // Ensure you have access to the MssAmigaScreen structure
    struct MssAmigaScreen *amigaScreen = mssAmigaScreen; // Obtain your amigaScreen reference here
	if (!amigaScreen) return 0;
	
	if (lastEvent!=0)
	{	
		event->type = 0;
		event->key = lastEvent->key;
		event->state = lastEvent->state;
		
		lastEvent = 0;
		
		return 1;
	}

	return 0;
}

extern "C" int MSS_PollEvent(struct MssEvent *event)
{
    struct IntuiMessage *msg;
	
	if (!mssAmigaScreen) return 0;
    if (!popEventFromQueue(event)) 
	{
        return 0;  // No events to process
    }
	return 1;


    return 0; // No events to process
}

extern "C" void MSS_FillRect(void *screen, int col, int x, int y, int width, int height)
{
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen *)mssAmigaScreen; // Assuming mssAmigaScreen is a global pointer to your MssAmigaScreen
    if (!amigaScreen || !amigaScreen->window) return;

    struct RastPort *rp = amigaScreen->window->RPort; // Get the RastPort from the window

    // Set the fill color
    SetAPen(rp, col); // Set the current pen color to the specified color

    // Fill the rectangle
    RectFill(rp, x, y, x + width - 1, y + height - 1); // Fill the rectangle
}

extern "C" void MSS_ShowCursor(int enable)
{
}

extern "C" void MSS_LockScreen(void *screen)
{
    struct MssAmigaScreen *mssScreen = (struct MssAmigaScreen *)screen;

    if (!mssScreen || !mssScreen->window) return;

    // Obtain the RastPort from the window
    struct RastPort *rp = mssScreen->window->RPort;
	    if (!rp || !rp->BitMap)
        return;

#ifdef USEGRAFFITI
    struct BitMap *bm = rp->BitMap;

    // Graffiti: expect chunky framebuffer at Planes[0] with valid BytesPerRow.
    if (!bm->Planes[0])
        return;

    mssScreen->videoMemoryAddress = (ULONG)bm->Planes[0];
    mssScreen->bytesPerRow        = (ULONG)bm->BytesPerRow;

    // Basic sanity: don't let a bogus pitch pass
    if (mssScreen->bytesPerRow < (ULONG)mssScreen->width)
    {
        // If this triggers, you're likely NOT on a Graffiti chunky bitmap.
        mssScreen->videoMemoryAddress = 0;
        mssScreen->bytesPerRow = 0;
        return;
    }
#else
    // AGA/other: original behavior (planar address of first bitplane)
    mssScreen->videoMemoryAddress = (ULONG)rp->BitMap->Planes[0];
#endif
}

extern "C" void MSS_UnlockScreen(void *screen)
{
}

struct Soff
{
 int x;
 int y;
};

//extern "C" void c2p1x1_8_c5_bm_040(int chunkyx __asm("d0"), int chunkyy __asm("d1"), int offsx __asm("d2"), int offsy __asm("d3"), void* c2pscreen __asm("a0"), struct BitMap* bitmap __asm("a1"));

extern "C" void MSS_DrawArray(void *screen, unsigned char* src, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int srcwidth, unsigned int dstwidth)
{
    struct MssAmigaScreen *amigaScreen = (struct MssAmigaScreen*)screen;
    if (!amigaScreen || !amigaScreen->window || !amigaScreen->window->RPort || !src)
        return;

    struct RastPort *rastPort = amigaScreen->window->RPort;

    // Compute destination start in the window bitmap coordinates.
    UWORD xstart = (UWORD)x;
    UWORD ystart = (UWORD)y;

    if (!amigaScreen->fullscreen)
    {
        xstart += amigaScreen->window->BorderLeft;
        ystart += amigaScreen->window->BorderTop;
    }

    UWORD xstop  = xstart + (UWORD)w - 1;
    UWORD ystop  = ystart + (UWORD)h - 1;

#ifdef USEGRAFFITI
    // Ensure MSS_LockScreen was called (or we can derive it here defensively)
    if (amigaScreen->videoMemoryAddress == 0 || amigaScreen->bytesPerRow == 0)
    {
        // Try to derive quickly (same logic as MSS_LockScreen)
        struct BitMap *bm = rastPort->BitMap;
        if (!bm || !bm->Planes[0]) return;

        amigaScreen->videoMemoryAddress = (ULONG)bm->Planes[0];
        amigaScreen->bytesPerRow        = (ULONG)bm->BytesPerRow;
        if (amigaScreen->bytesPerRow < (ULONG)amigaScreen->width) return;
    }

    UBYTE *dst = (UBYTE*)amigaScreen->videoMemoryAddress
               + (ULONG)ystart * amigaScreen->bytesPerRow
               + (ULONG)xstart;

    // Copy line by line (pitch-aware). We ignore dstwidth because Graffiti pitch is bytesPerRow.
    for (unsigned int row = 0; row < h; row++)
    {
        memcpy(dst, src, w);
        src += srcwidth;
        dst += amigaScreen->bytesPerRow;
    }
    return;
#endif

    // ---- Original logic below (AGA/RTG paths) ----
    if (!usingAGA)
    {
        WriteChunkyPixels(rastPort, xstart, ystart, xstop, ystop, src, amigaScreen->width);
        return;
    }

    if (w != 320)
    {
        WriteChunkyPixels(rastPort, xstart, ystart, xstop, ystop, src, amigaScreen->width);
    }
    else
    {
        if (amigaScreen->fullscreen == 0) usingWCP = 1;
        if (!usingWCP)
        {
            // c2p path (disabled in your file right now)
            // c2p1x1_8_c5_bm_040(w, h, 0, 0, src, rastPort->BitMap);
        }
        else
        {
            WriteChunkyPixels(rastPort, xstart, ystart, xstop, ystop, src, amigaScreen->width);
        }
    }
}

extern "C" void* MSS_GetWindow(void *screen) 
{
	if (mssAmigaScreen==0) return 0;
	
	return mssAmigaScreen->window;
}	