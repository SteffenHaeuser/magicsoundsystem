#include <SDL/SDL.h>
#include <vorbis/codec.h>   
#include <vorbis/vorbisfile.h> 
#include <vector>
#include <algorithm>
#include <timidity/timidity.h>
#include <proto/mpega.h>
#include <libraries/mpega.h>
#include <proto/exec.h>

char __attribute__((used)) stackcookie[] = "$STACK: 2000000";
const char *version_tag = "$VER: 1.7 MagicSoundSystem.dll (01.06.2024) by Steffen \"MagicSN\" Haeuser";

#include "audio_utils.h"
#include "magicsoundsystem_oggfile.h"

#define MSS_TYPE_OGG 0
#define MSS_TYPE_MP3 1
#define MSS_TYPE_AIFF 2

#include "dll.h"

extern "C" typedef int (*MSS_OpenWaveFunc)(const char*, unsigned char**, unsigned int*, int);
extern "C" typedef void (*MSS_CloseWaveFunc)(void*, void*);
#ifndef ONLYCVERSION
extern "C" typedef int (*MSS_OpenWaveAmigaFunc)(const char*, void*&, int&);
#endif

bool soundOn = true;
bool ENABLE_SOUND;
int g_FXInitialized = 0;
float mvolume = 1.0f;
int thechannels;
int thefrequency;
int theformat;
int thesamples;
int streamThreshold = 1024 * 1024;

struct Library *MPEGABase = 0;

MSS_OpenWaveFunc MSS_OpenWave_fct = 0;
MSS_CloseWaveFunc MSS_CloseWave_fct = 0;
#ifndef ONLYCVERSION
MSS_OpenWaveAmigaFunc MSS_OpenWaveAmiga_fct = 0;
#endif
bool MSS_UseNewLoader=false;

Uint16 to_big_endian16(Uint16 little_endian)
{
    return (little_endian >> 8) | (little_endian << 8);
}

Uint32 to_big_endian32(Uint32 little_endian) 
{
    return ((little_endian >> 24) & 0x000000FF) |
           ((little_endian >> 8)  & 0x0000FF00) |
           ((little_endian << 8)  & 0x00FF0000) |
           ((little_endian << 24) & 0xFF000000);
}

typedef struct {
    FILE *file;              // File pointer for the WAV file
    unsigned int dataChunkPos;     // Position of the data chunk in the file
    unsigned int dataSize;         // Size of the data chunk
} WavStreamData;

int MSS_OpenWave(const char* name, unsigned char** vbuffer, unsigned int* size, int UsedInWindowsOnly) 
{
    FILE* file = fopen(name, "rb"); // Open the WAV file in binary mode for reading
    if (!file) 
	{
        return 0;
    }

    // Seek to the end of the file to get its size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET); // Reset file position to the beginning

    // Allocate memory to store the WAV file content
    *vbuffer = (unsigned char*)malloc(fileSize);
    if (!(*vbuffer)) 
	{
        fclose(file);
        return 0;
    }

    // Read the entire file into the buffer
    size_t bytesRead = fread(*vbuffer, 1, fileSize, file);
    if (bytesRead != fileSize) 
	{
        fclose(file);
        free(*vbuffer);
        return 0;
    }

    fclose(file);

    // Pass the loaded data and size to the caller
    *size = fileSize;
    return 1;
}

void MSS_CloseWave(void* audioBuffer, void *fileBuffer) 
{
    if (audioBuffer) 
	{
        free(audioBuffer);
        audioBuffer = 0;
    }
}

class SoundItem 
{
public:
    SDL_AudioSpec spec;
    Uint8* audioBuffer;
    Uint32 audioLength;
    int position;
	bool playing;
    double vol;
    double pan; 	
	bool looped;
	float playtime;
	int stopit;
	int isogg;
	int ismidi;
	bool ismp3;
	int wavstreamfreq;
	int wavstreamchannels;
	bool usewavstreaming;
	WavStreamData *extradata;
	int midiMaxChunkSize;
	OggVorbis_File *ogg_file;
	unsigned char *extFile;
	MSS_OggFile *theOggFile;
	vorbis_info *info;
	FILE *thelog;
	unsigned char *midiBuffer;
	MidIStream *midiStream;
	MidSong *midiSong;
	unsigned char* mp3_buffer;
    size_t mp3_buffer_size;
    MPEGA_STREAM* mp3_stream;
	SoundItem()
	{
			midiSong = 0;
			midiStream = 0;
			audioBuffer = 0;
			extFile = 0;
			audioLength = 0;
			position = 0;
			playing = false;
			vol = 1.0f;
			pan = 0.0f;
			thelog = 0;
			stopit = 0;
			ogg_file = 0;
			theOggFile = 0;
			info = 0;
			ismidi = 0;
			ismp3 = 0;
			mp3_buffer = 0;
			mp3_buffer_size = 0;
			mp3_stream = 0;
			midiMaxChunkSize = 0;
			midiBuffer = 0;
			usewavstreaming = 0;
			extradata = 0;
			wavstreamfreq = 0;
			wavstreamchannels = 0;
	}
};

std::vector<SoundItem*> g_FXsounds;

extern "C" void MSS_SetLog(void *handle, FILE *thelog)
{
	SoundItem *sound = (SoundItem*)handle;
	if (!sound) return;
	sound->thelog = thelog;
}

extern "C" int MSS_IsPlaying(void *handle)
{
	SoundItem *sound = (SoundItem *)handle;
	if (sound)
	{
		if (sound->stopit) 
		{
			sound->playing = 0;
		}
		return sound->playing;	
	}
	else return 0;
}

void AudioCallback_Ogg(SoundItem *sound, void *userdata, unsigned char *stream, int length) 
{
    int bitstream;
    long read = ov_read(sound->ogg_file, reinterpret_cast<char *>(stream), length, 0, 2, 1, &bitstream);
    if (read <= 0) 
    {
        // Handle end of file
        if (sound->looped) 
        {
            // Rewind to the beginning of the file
            ov_pcm_seek(sound->ogg_file, 0);
            sound->position = 0;
        } 
        else 
        {
            // Stop playback
            sound->stopit = 1;
			sound->playing = 0;
        }
    } 
    else 
    {

        sound->position += read;

        // Perform channel conversion if necessary
        if ((thechannels != sound->info->channels)||(sound->info->rate != thefrequency)) 
        {
            SDL_AudioCVT cvt;
            SDL_BuildAudioCVT(&cvt, AUDIO_S16SYS, sound->info->channels, sound->info->rate, AUDIO_S16SYS, thechannels, thefrequency);
            cvt.buf = reinterpret_cast<unsigned char *>(stream);
            cvt.len = read;
            SDL_ConvertAudio(&cvt);
            read = cvt.len_cvt;
        }                   
        // Mix the decoded audio chunk into the stream
        SDL_MixAudio(stream, reinterpret_cast<unsigned char *>(stream), read, (SDL_MIX_MAXVOLUME*sound->vol));
    }   
}

void AudioCallback_MP3(SoundItem *sound, void *userdata, unsigned char *stream, int length) 
{
    // For MP3 files, decode and mix audio data from stream
	WORD *pcm[MPEGA_MAX_CHANNELS] = { nullptr, nullptr };
    LONG samples = MPEGA_decode_frame(sound->mp3_stream, pcm);
    if (samples > 0) 
    {
        SDL_AudioCVT cvt;
        if ((thechannels != sound->mp3_stream->channels) || (sound->mp3_stream->frequency != thefrequency)) 
        {						
            SDL_BuildAudioCVT(&cvt, AUDIO_S16SYS, sound->mp3_stream->channels, sound->mp3_stream->frequency, AUDIO_S16SYS, thechannels, thefrequency);
            cvt.buf = reinterpret_cast<unsigned char *>(sound->mp3_buffer);
            cvt.len = samples * sound->mp3_stream->channels * sizeof(short);
            SDL_ConvertAudio(&cvt);

            // Mix the decoded audio chunk into the stream
            SDL_MixAudio(stream, cvt.buf, cvt.len_cvt, (SDL_MIX_MAXVOLUME*sound->vol));
        } 
        else 
        {
            // No conversion needed, mix directly
            SDL_MixAudio(stream, reinterpret_cast<unsigned char *>(pcm[0]), samples * sizeof(WORD), (SDL_MIX_MAXVOLUME*sound->vol));
        }

        // Update sound position
        sound->position += samples * sound->mp3_stream->channels * sizeof(short);
        if (sound->looped && sound->position >= sound->audioLength) 
        {
            MPEGA_seek(sound->mp3_stream, 0);
            sound->position = 0;
        } 
        else if (!sound->looped && sound->position >= sound->audioLength) 
        {
            sound->stopit = 1;
        }
    } 
    else if (samples == MPEGA_ERR_EOF) 
    {
        if (sound->looped) 
        {
            MPEGA_seek(sound->mp3_stream, 0);
            sound->position = 0;
        } 
        else 
        {
            sound->stopit = 1;
        }
    } 
    else if (samples == MPEGA_ERR_BADFRAME) 
    {
    }	
}

void AudioCallback_Wav(SoundItem *sound, void *userdata, unsigned char *stream, int length) 
{
    if (sound->usewavstreaming) 
	{
        WavStreamData *streamData = (WavStreamData *)sound->extradata;

        // Check if we reached the end of the file
        if (sound->position >= sound->audioLength) 
		{
            // If looped, reset position to loop the sound
            if (sound->looped) 
			{
                sound->position = 0;
                fseek(streamData->file, streamData->dataChunkPos, SEEK_SET);
            } 
			else 
			{
                sound->stopit = 1;
                sound->playing = 0;
                return;
            }
        }

        // Calculate remaining bytes to read
        int remainingBytes = sound->audioLength - sound->position;

        // Read data from the file
        int bytesRead = fread(stream, 1, length, streamData->file);
        if (bytesRead < length) 
		{
            // If we didn't read enough, fill the rest with silence
            memset(stream + bytesRead, 0, length - bytesRead);

            // If looped, reset position to loop the sound
            if (sound->looped) 
			{
                sound->position = 0;
                fseek(streamData->file, streamData->dataChunkPos, SEEK_SET);
            } 
			else 
			{
                sound->stopit = 1;
                sound->playing = 0;
                return;
            }
        }
		
		if ((sound->wavstreamchannels != thechannels) || (sound->wavstreamfreq !=thefrequency))
		{
            SDL_AudioCVT cvt;
            SDL_BuildAudioCVT(&cvt, AUDIO_S16, sound->wavstreamchannels, sound->wavstreamfreq, AUDIO_S16, thechannels, thefrequency);
            cvt.buf = stream;
            cvt.len = bytesRead;
            SDL_ConvertAudio(&cvt);
            bytesRead = cvt.len_cvt;
        }		

		SDL_MixAudio(stream, stream, bytesRead, (SDL_MIX_MAXVOLUME*sound->vol));
        // Update position
        sound->position += bytesRead;
    } 
	else 
	{
        // Non-streaming playback logic
        if (sound->position >= sound->audioLength - length) 
		{
            sound->position += length;
            if (sound->looped && sound->position >= sound->audioLength) 
			{
                sound->position = 0; // Reset position to loop the sound
            } 
			else if (!sound->looped && sound->position >= sound->audioLength) 
			{
                sound->stopit = 1;
                sound->playing = 0;
            }
            return;
        }	
        SDL_MixAudio(stream, sound->audioBuffer + sound->position, length, (SDL_MIX_MAXVOLUME*sound->vol));
        sound->position += length;
        if (sound->looped && sound->position >= sound->audioLength) 
		{
            sound->position = 0; // Reset position to loop the sound
        } else if (!sound->looped && sound->position >= sound->audioLength) 
		{
            sound->stopit = 1;
            sound->playing = 0;
        }
    }
}

void AudioCallback_Midi(SoundItem *sound, void *userdata, unsigned char *stream, int length) 
{
    int bytesRead = mid_song_read_wave(sound->midiSong, sound->midiBuffer, length);
    if ((thechannels != 2) || (44100 != thefrequency)) 
    {
        SDL_AudioCVT cvt;
        SDL_BuildAudioCVT(&cvt, AUDIO_S16, 2, 44100, AUDIO_S16, thechannels, thefrequency);
        cvt.buf = reinterpret_cast<uint8_t*>(sound->midiBuffer);
        cvt.len = bytesRead;
        SDL_ConvertAudio(&cvt);
        bytesRead = cvt.len_cvt;
    }
    SDL_MixAudio(stream, sound->midiBuffer, bytesRead, (SDL_MIX_MAXVOLUME*sound->vol));
    sound->position += bytesRead;
    if (sound->looped && sound->position >= sound->audioLength) 
    {
        mid_song_start(sound->midiSong);
        sound->position = 0;
    } 
    else if (!sound->looped && sound->position >= sound->audioLength) 
    {
        sound->stopit = 1;
    }   
}

void AudioCallback(void *userdata, unsigned char *stream, int len) 
{
    SDL_memset(stream, 0, len);

    for (int i = 0; i < g_FXsounds.size(); ++i) 
	{
        SoundItem *sound = g_FXsounds[i];
        if (MSS_IsPlaying((void *)sound)) 
		{
            int remaining = sound->audioLength - sound->position;
            if (remaining > 0) 
			{
                int length = (remaining < len) ? remaining : len;
                if (sound->isogg) 
				{
					AudioCallback_Ogg(sound, userdata,stream,len);
                }
				else if (sound->ismidi) 
				{
					AudioCallback_Midi(sound, userdata,stream,len);
				}		
                else if (sound->ismp3)
                {
					AudioCallback_MP3(sound, userdata,stream,len);
                }                				
				else 
				{
					AudioCallback_Wav(sound, userdata,stream,len);
                }
            }
        }
    }
}

extern "C" void MSS_SetAudioSystemLoaders(MSS_OpenWaveFunc loadWav, MSS_CloseWaveFunc freeWav) 
{
        MSS_OpenWave_fct = loadWav;
        MSS_CloseWave_fct = freeWav;
#ifndef ONLYCVERSION
		MSS_OpenWaveAmiga_fct = 0;
#endif		
        MSS_UseNewLoader = false;
}
					
int mid_inited = 0;
extern "C" void MSS_SetWAVDirectory(const char* dirname1, const char* dirname2);
					
extern "C" int MSS_SoundInit(int frequency)
{
    SDL_AudioSpec spec;
	
	if ((!ENABLE_SOUND) || soundOn==false) return 0;
	
	MSS_SetWAVDirectory(0,0);
	
	if ((frequency!=22050)&&(frequency!=44100)) return 0;

	if (!MPEGABase)
	{
		if (!(MPEGABase = OpenLibrary("mpega.library", 0)))
		{
		}
	}

	g_FXInitialized = 0;
    if (SDL_Init(SDL_INIT_AUDIO) < 0) 
	{
        return 0;
    }
	
	frequency = frequency;
	
    spec.freq = frequency;
    spec.format = AUDIO_S16;
    spec.channels = 2;
    if (frequency == 22050) spec.samples = 512; 
	else spec.samples = 1024;
    spec.callback = AudioCallback;
    spec.userdata = 0;
	thechannels = 2;
	thefrequency = frequency;
	thesamples = spec.samples;
	theformat = spec.format;

    if (!g_FXInitialized)
    {	
        if (SDL_OpenAudio(&spec, NULL) < 0) 
        {
            return 0;
        }
        g_FXInitialized = 1;
    }
	
	mid_inited = 0;
    if (mid_init(NULL) < 0) 
	{
		int mid_inited = 1;
        return 1;
    }	
		
    return 1;
}

extern "C" void MSS_SoundClose()
{
	if ((!ENABLE_SOUND) || soundOn==false) return;
	
	if (mid_inited) mid_exit();	
	
	for (auto soundPtr : g_FXsounds) 
	{
		if (MSS_CloseWave_fct==0)
		{
			MSS_CloseWave(soundPtr->audioBuffer, soundPtr->audioBuffer);
		}
		else
		{
			MSS_CloseWave_fct(soundPtr->audioBuffer, soundPtr->audioBuffer);
		}
		delete soundPtr;
    }
	g_FXsounds.clear();	
	SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
	if (MPEGABase)
	{
		CloseLibrary(MPEGABase);
		MPEGABase = 0;
	}
	g_FXInitialized = 0;
}
	
extern "C" void MSS_SoundOnOff(int on)
{
	ENABLE_SOUND = 1;
	soundOn = on && ENABLE_SOUND;
}

extern "C" int MSS_GetSoundOnOff()
{
	return soundOn && ENABLE_SOUND;
}

extern "C" void MSS_SetVolume(void *handle, double _vol)
{
	SoundItem *sound = (SoundItem*)handle;
    
	if (sound)
	{
		sound->vol = _vol;	
	}
}

extern "C" void MSS_SetPan(void *handle, double _pan)
{
	SoundItem *sound = (SoundItem*)handle;
	
	if (sound)
	{
		const int HALF_PAN = 127;

		_pan=(int)((1.0+_pan)*63);
   
		sound->pan = _pan;	
	}
}

extern "C" double MSS_GetMusicVolume()
{	
	return mvolume;
}		

extern "C" void MSS_SetMusicVolume(double mvol)
{
	if (mvol > 1) mvol = 1;
	else if (mvol < 0) mvol = 0;
	mvolume = mvol;
}

extern "C" void MSS_Play(void *handle, double _vol, double _pan, int looped, bool ignoreGlobalVolume)
{
	if ((!ENABLE_SOUND) || soundOn==false) return;
	
	SoundItem *sound = (SoundItem*)handle;
				
	if (sound)
	{
		if (ignoreGlobalVolume==false) 
		{
			_vol*=mvolume;
		}
		if (_vol<=0.0) return;		
		sound->vol = _vol;
		sound->pan = _pan;
		sound->position = 0;	
		sound->looped = (looped != 0);
		SDL_PauseAudio(0);
		sound->playing = true;	
	}
}

extern "C" void MSS_Stop(void *handle)
{
	SoundItem *sound = (SoundItem*)handle;
	if (sound)
	{
		sound->playing = false;
	}
}

extern "C" void MSS_Free(void *handle)
{
	SoundItem *sound = (SoundItem *)handle;
	MSS_Stop(handle);
	if (sound)
	{
		if (sound->isogg)
		{
			sound->playing = 0;
			if (MSS_CloseWave_fct==0)
			{
				MSS_CloseWave(sound->audioBuffer, sound->audioBuffer);
			}
			else
			{
				MSS_CloseWave_fct(sound->audioBuffer, sound->audioBuffer);
			}			
			if (sound->ogg_file)
			{
				ov_clear(sound->ogg_file);
				delete sound->ogg_file;			
				sound->ogg_file = 0;
			}						
			if (sound->extFile)
			{
				free(sound->extFile);
				sound->extFile = 0;
			}
			sound->audioBuffer = 0;
			if (sound->usewavstreaming) 
			{
				WavStreamData *streamData = (WavStreamData *)sound->extradata;
				if (streamData) 
				{
					if (streamData->file) fclose(streamData->file);
					streamData->file = 0;
					free(streamData);
					sound->extradata = 0;
				}
			}			
			g_FXsounds.erase(std::remove(g_FXsounds.begin(), g_FXsounds.end(), sound), g_FXsounds.end());	
			delete sound;	
			sound = 0;
			return;
		}
		else if (sound->ismidi)
		{
			mid_song_free(sound->midiSong);
			mid_istream_close(sound->midiStream);
			g_FXsounds.erase(std::remove(g_FXsounds.begin(), g_FXsounds.end(), sound), g_FXsounds.end());		
			sound->playing = 0;			
			free(sound->midiBuffer);
			sound->midiBuffer = 0;
			if (sound->extFile) free(sound->extFile);
			sound->extFile = 0;
			sound->audioBuffer = 0;
			if (sound) delete sound;
			sound = 0;
			return;
		}
		else if (sound->ismp3)
		{
			g_FXsounds.erase(std::remove(g_FXsounds.begin(), g_FXsounds.end(), sound), g_FXsounds.end());		
			sound->playing = 0;			
			if (sound->mp3_stream) 
			{
				MPEGA_close(sound->mp3_stream);
				sound->mp3_stream = 0;
			}

			if (sound->mp3_buffer) 
			{
				free(sound->mp3_buffer);
				sound->mp3_buffer = 0;
			}			
			if (sound) delete sound;
			sound = 0;
			return;			
		}
		g_FXsounds.erase(std::remove(g_FXsounds.begin(), g_FXsounds.end(), sound), g_FXsounds.end());		
		sound->playing = 0;
		if (MSS_CloseWave_fct==0)
		{
			MSS_CloseWave(sound->audioBuffer, sound->audioBuffer);
		}
		else
		{
			MSS_CloseWave_fct(sound->audioBuffer, sound->audioBuffer);
		}
		sound->audioBuffer = 0;
	}
}

extern "C" void MSS_ConvertAudioFormat(void *handle)
{
	SoundItem *sound;
	SDL_AudioCVT cvt;
	
	sound = (SoundItem*)handle;	
	
	if (!sound) return;	
	
	SDL_BuildAudioCVT(&cvt, sound->spec.format, sound->spec.channels, sound->spec.freq, AUDIO_S16, thechannels, thefrequency);

	if (cvt.needed) 
	{
		cvt.buf = reinterpret_cast<Uint8*>(malloc(sound->audioLength * cvt.len_mult));
		if (!cvt.buf) 
		{
			if (sound->thelog) fprintf(sound->thelog, "Failed to allocate memory for audio conversion\n");
			return;
		}
		memcpy(cvt.buf, sound->audioBuffer, sound->audioLength);

		cvt.len = sound->audioLength;
		if (SDL_ConvertAudio(&cvt) < 0) 
		{
			if (sound->thelog) fprintf(sound->thelog, "Audio conversion failed: %s\n", SDL_GetError());
			free(cvt.buf);
			return;
		}

		sound->spec.format = theformat;
		sound->spec.channels = thechannels;
		sound->spec.freq = thefrequency;

		// Free the original audio buffer and update it with the converted buffer
		free(sound->audioBuffer);
		sound->audioBuffer = cvt.buf;
		sound->audioLength = cvt.len_cvt;
	}	
}

extern "C" void *MSS_LoadStreamFromMemory(void *mem_ptr, int len, int stream_type) 
{
	if (stream_type == MSS_TYPE_MP3)
	{
        // Initialize SoundItem
        SoundItem *sound = new SoundItem();
        sound->mp3_buffer = 0;
        sound->mp3_buffer_size = 0;
        MPEGA_CTRL ctrl = {0}; 

		MPEGA_STREAM *mp3_stream = MPEGA_open((char *)mem_ptr, &ctrl);
        if (!mp3_stream) 
		{
            delete sound;
            return 0;
        }
		
        WORD *pcm[MPEGA_MAX_CHANNELS] = { nullptr, nullptr };
        LONG samples = MPEGA_decode_frame(mp3_stream, pcm);
        if (samples < 0)
		{
            MPEGA_close(mp3_stream);
            delete sound;
            return 0;
        }		

        // Set the audio format
        sound->spec.format = AUDIO_S16SYS;
        sound->spec.channels = mp3_stream->channels;
        sound->spec.freq = mp3_stream->frequency;
        sound->ismp3 = 1;
        sound->position = 0;

        // Allocate buffer for decoded PCM data
		sound->mp3_buffer_size = samples * mp3_stream->channels * sizeof(short);
        sound->mp3_buffer = (unsigned char *)malloc(sound->mp3_buffer_size);
        if (!sound->mp3_buffer) 
		{
            MPEGA_close(mp3_stream);
            delete sound;
            return 0;
        }

        // Calculate the total audio length
		sound->audioLength = static_cast<unsigned int>((len * 8) / (mp3_stream->bitrate * 1000) * mp3_stream->frequency * mp3_stream->channels * sizeof(short));
        sound->playtime = static_cast<float>(len) / mp3_stream->bitrate;
		
        if (MPEGA_seek(mp3_stream, 0) != 0) 
	    {
            MPEGA_close(mp3_stream);
            free(sound->mp3_buffer);
            delete sound;
            return 0;
        }		

        // Store the mp3_stream in the sound item for further processing
        sound->mp3_stream = mp3_stream;

        // Add sound to g_FXsounds vector
        g_FXsounds.push_back(sound);

        return sound;		
	}
	else if (stream_type == MSS_TYPE_AIFF)
	{
		return 0;
	}
	else if (stream_type != MSS_TYPE_OGG)
	{
		return 0;
	}
	
    // Initialize SoundItem
    SoundItem *sound = new SoundItem();
    sound->ogg_file = new OggVorbis_File;
		
	ov_callbacks callbacks = 
	{
        .read_func = MSS_readOgg,
        .seek_func = MSS_seekOgg,
        .close_func = MSS_closeOgg,
        .tell_func = MSS_tellOgg
    };	

    // Open the OGG Vorbis file from memory
    MSS_OggFile *t = new MSS_OggFile;
	sound->theOggFile = t;
    t->curPtr = t->filePtr = (char *)mem_ptr;
    t->fileSize = len;

    // Open the OGG Vorbis file from memory
    int ret = ov_open_callbacks((void *)t, sound->ogg_file, nullptr, 0, callbacks);
    if (ret < 0) {
        delete sound->ogg_file;
        delete sound;
		delete t;		
        return nullptr;
    }

    // Get the OGG Vorbis file info
    sound->info = ov_info(sound->ogg_file, -1);

    if (!sound->info) {
        ov_clear(sound->ogg_file);
        delete sound->ogg_file;
        delete sound;
		delete t;
        return nullptr;
    }

    // Set the audio format
    sound->spec.format = AUDIO_S16SYS;
    sound->spec.channels = sound->info->channels;
    sound->spec.freq = sound->info->rate;
	sound->isogg = 1;
	sound->position = 0;
	sound->audioLength = static_cast<unsigned int>(ov_pcm_total(sound->ogg_file, -1) * sound->info->channels * sizeof(short));
	long totalSamples = ov_pcm_total(sound->ogg_file, -1);
    sound->playtime = static_cast<float>(totalSamples) / sound->info->rate;
	
	// Add sound to g_FXsounds vector
	g_FXsounds.push_back(sound);	

    return sound;
}	

extern "C" char* MSS_LoadFileToMemory(const char* filename, size_t& fileSize);

void *MSS_LoadWave(const char* name, unsigned int* loadedSize) {
    unsigned char* tempBuffer = nullptr;
    unsigned int tempSize = 0;
	int theSize = 0;

    if (MSS_UseNewLoader) {
        if (MSS_OpenWaveAmiga_fct) {
            void* tempVoidBuffer = nullptr;
            if (MSS_OpenWaveAmiga_fct(name, tempVoidBuffer, theSize) == 1) {
                unsigned char* buffer = static_cast<unsigned char*>(tempVoidBuffer);
                *loadedSize = static_cast<unsigned int>(theSize);
                return buffer;
            }
        } else {
            if (MSS_OpenWave(name, &tempBuffer, &tempSize, 0) == 1) {
                *loadedSize = static_cast<int>(tempSize);
                return tempBuffer;
            }
        }
    } else {
        if (MSS_OpenWave_fct) {			
            if (MSS_OpenWave_fct(name, &tempBuffer, &tempSize, 0) == 1) {
                *loadedSize = static_cast<int>(tempSize);
				return tempBuffer;
            }		
        } else {
            if (MSS_OpenWave(name, &tempBuffer, &tempSize, 0) == 1) {
                *loadedSize = static_cast<int>(tempSize);
                return tempBuffer;
            }
        }
    }
    return 0;
}

extern "C" char* MSS_LoadFileToMemory(const char* filename, size_t& fileSize)
{
    FILE* file = fopen(filename, "rb");
    if (!file) 
	{
        fileSize = 0;
        return 0;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory buffer
    char* buffer = (char*)malloc(size);

    // Read file contents into buffer
    size_t bytesRead = fread(buffer, 1, size, file);
    fclose(file);

    // Ensure that the entire file was read
    if (bytesRead != size) 
	{
        delete[] buffer;
        fileSize = 0;
        return 0;
    }

    fileSize = size;
    return buffer;
}

int MSS_UseSDLWAVLoader = 0;
char MSS_WAVPath[8192];
char MSS_WAVPath2[8192];

extern "C" void MSS_SetWAVDirectory(const char* dirname1, const char* dirname2)
{
	if (dirname1==0)
	{
		MSS_UseSDLWAVLoader = 0;
		return;
	}
	MSS_UseSDLWAVLoader = 1;
	strcpy(MSS_WAVPath,dirname1);
	if (dirname2==0)
	{
		strcpy(MSS_WAVPath2,MSS_WAVPath);
	}
	else 
	{
		strcpy(MSS_WAVPath2,dirname2);
	}
}

int LoadWAVStreaming(const char *file, SoundItem *sound) 
{
    // Open the WAV file
    FILE *wavFile = fopen(file, "rb");
    if (!wavFile) 
	{
        fprintf(stderr, "Failed to open WAV file: %s\n", file);
        return 0;
    }

    // Read the WAV header
    unsigned char wavHeader[44];
    if (fread(wavHeader, 1, 44, wavFile) != 44) 
	{
        fprintf(stderr, "Failed to read WAV header\n");
        fclose(wavFile);
        return 0;
    }

    // Parse the WAV header (simplified) and convert to Big Endian
    unsigned short audioFormat = to_big_endian16(*(unsigned short *)(wavHeader + 20));
    unsigned short numChannels = to_big_endian16(*(unsigned short *)(wavHeader + 22));
    unsigned int sampleRate = to_big_endian32(*(unsigned int *)(wavHeader + 24));
    unsigned short bitsPerSample = to_big_endian16(*(unsigned short *)(wavHeader + 34));
    unsigned int dataChunkSize = to_big_endian32(*(unsigned int *)(wavHeader + 40));

    if (audioFormat != 1) 
	{ // PCM
        fprintf(stderr, "Unsupported WAV format\n");
        fclose(wavFile);
        return 0;
    }
	
	sound->wavstreamchannels = numChannels;
	sound->wavstreamfreq = sampleRate;

    // Allocate memory for WavStreamData
    WavStreamData *streamData = (WavStreamData *)malloc(sizeof(WavStreamData));
    if (!streamData) 
	{
        fprintf(stderr, "Failed to allocate memory for stream data\n");
        fclose(wavFile);
        return 0;
    }

    // Initialize WavStreamData
    streamData->file = wavFile;
    streamData->dataChunkPos = 44; // Assuming a standard 44-byte WAV header
    streamData->dataSize = dataChunkSize;

    // Initialize the SoundItem for streaming
    sound->audioBuffer = NULL;  // Not used for streaming
    sound->audioLength = dataChunkSize;
    sound->position = 0;
    sound->looped = 0;
    sound->stopit = 0;
    sound->playing = 1;
    sound->usewavstreaming = 1;
    sound->extradata = streamData;

    return 1;
}

extern "C" void MSS_SetStreamThreshold(int threshold)
{
	streamThreshold = threshold;
}

extern "C" void *MSS_LoadSample(const char* name)
{
	SoundItem *sound = 0;
	int freq, channels, bitsPerSample;	
	unsigned int loadedSize;
	unsigned char* loadedBuffer = 0;
		
	if ((!ENABLE_SOUND) || soundOn==false) return 0;

	sound = new SoundItem();
	
	if (!sound) return 0;
	
	sound->stopit = 0;
	sound->isogg = 0;
	sound->ismidi = 0;
	sound->ismp3 = 0;
	sound->playing = 0;
	
	long fileSize = 0;

	if ((strstr(name, ".wav"))||(strstr(name,".WAV")))
	{
		MSS_UseSDLWAVLoader = 1;
		if (MSS_UseSDLWAVLoader == 1)
		{
			SDL_AudioSpec wavSpec;
			Uint8 *wavBuffer;
			Uint32 wavLength;
			FILE *fil;
			char wavpath[16384];
		
			strcpy(wavpath,MSS_WAVPath);
			if ((wavpath[strlen(wavpath)-1]!='/')&&(wavpath[strlen(wavpath)-1]!=':'))
			{
				strcat(wavpath,"/");
			}
			
			strcat(wavpath,name);	
			
			fil = fopen(wavpath,"r");
			if (!fil)
			{
				strcpy(wavpath,MSS_WAVPath2);
				if ((wavpath[strlen(wavpath)-1]!='/')&&(wavpath[strlen(wavpath)-1]!=':'))
				{
					strcat(wavpath,"/");
				}
			
				strcat(wavpath,name);							
			}
			else
			{
				fclose(fil);
			}
			
			FILE *thefile = fopen(wavpath, "rb");
			if (!thefile) 
			{
				delete sound;
				return 0;
			}
			fseek(thefile, 0, SEEK_END);
			long fileSize = ftell(thefile);
			fclose(thefile);				
			
			if ((fileSize > streamThreshold) && (streamThreshold != 0))
			{ 
				if (LoadWAVStreaming(wavpath, sound) != 1) 
				{
					delete sound;
					return 0;
				}
				loadedSize = sound->audioLength;
				freq = sound->wavstreamfreq;
				channels = sound->wavstreamchannels;
				bitsPerSample = 16;
				loadedBuffer = 0;
			}	
			else
			{
				if (SDL_LoadWAV(wavpath, &wavSpec, &wavBuffer, &wavLength) == NULL)
				{
					fprintf(stderr, "LoadWAV failed: %s\n", SDL_GetError());
					delete sound;
					return 0;
				}

				freq = wavSpec.freq;
				channels = wavSpec.channels;
				bitsPerSample = 16;
				loadedBuffer = wavBuffer;
				loadedSize = wavLength;
				sound->audioBuffer = (unsigned char*)malloc(loadedSize);
				if (!sound->audioBuffer)
				{
					SDL_FreeWAV(wavBuffer);
					delete sound;
					return 0;
				}
				memcpy(sound->audioBuffer, loadedBuffer, loadedSize);
				sound->audioLength = loadedSize;
				SDL_FreeWAV(wavBuffer);
			}
		}
		else
		{
			loadedBuffer = (unsigned char*)MSS_LoadWave(name,&loadedSize);
			if (!loadedBuffer) 
			{
				delete sound;
				return 0;
			}		
	
			if (!MSS_AnalyzeWAVHeader(loadedBuffer, loadedSize, &freq, &channels, &bitsPerSample)) 
			{
				delete sound;
				return 0;
			}
		
			if (!MSS_LoadAudioFromBuffer(loadedBuffer, loadedSize, &(sound->audioBuffer), &(sound->audioLength))) 
			{
				delete sound;
				return 0;
			}
		}
	}
	else if ((strstr(name, ".aiff"))||(strstr(name,".AIFF")))
	{
		delete sound;
		return 0;
	}
	else if ((strstr(name, ".midi"))||(strstr(name,".MIDI"))||(strstr(name, ".mid"))||(strstr(name,".MID")))
	{
		MidSongOptions options; 
		SoundItem *sound = new SoundItem();
		char *midiData;
		size_t fileSize = 0;
		int ticksPerBeat;
		float tempo;
        
		if (!sound)
		{
			return 0;
		}			
		
        midiData = MSS_LoadFileToMemory(name, fileSize);
        if (midiData == 0 || fileSize == 0) 
		{
            delete sound;
            return 0;
        }		

		sound->extFile = (unsigned char*)midiData;

		sound->midiStream = mid_istream_open_mem(midiData, fileSize, 1);
		if (!sound->midiStream) 
		{
			free(midiData);
			delete sound;
			return 0;
		}
		
		options.rate = 44100;
		options.format = MID_AUDIO_S16;
		options.channels = 2;
		options.buffer_size = 1024;
		channels = 2;

		sound->midiSong = mid_song_load(sound->midiStream, &options);
		if (!sound->midiSong) 
		{
			mid_istream_close(sound->midiStream);
			free(midiData);
			delete sound;
			return 0;
		}

		sound->audioLength = mid_song_get_total_time(sound->midiSong) * 44100 * 2 * sizeof(int16_t);
		sound->position = 0;		

		sound->ismidi = 1;
		sound->midiMaxChunkSize = options.buffer_size * options.channels * (options.format == MID_AUDIO_S16 ? 2 : 1);
		sound->midiBuffer = (unsigned char *)malloc(sound->midiMaxChunkSize);		
	}
	else
	{
		delete sound;
		return 0;
	}

	sound->spec.format = (bitsPerSample == 8) ? AUDIO_U8 : AUDIO_S16;	
	sound->spec.channels = channels;
	sound->spec.freq = freq;

    sound->spec.callback = AudioCallback;
    sound->spec.userdata = sound;
	sound->playing = false;	
	
	int bytesPerSample = bitsPerSample / 8;
    long totalSamples = sound->audioLength / (channels * bytesPerSample);
    sound->playtime = static_cast<float>(totalSamples) / freq;	

	if (sound->ismidi==0)
	{
		MSS_ConvertAudioFormat(sound);	

		sound->spec.freq = thefrequency;
		sound->spec.channels = thechannels;
		sound->spec.format = theformat;	
	}
	
	g_FXsounds.push_back(sound);

	return sound;
}

extern "C" void *MSS_LoadAudioFromMemory(const char* audioBuffer, unsigned int audioLength, int channels, int freq, int bitsPerSample)
{
	SoundItem *sound = 0;
	unsigned int loadedSize;
	unsigned char* loadedBuffer = 0;		
		
	if ((!ENABLE_SOUND) || soundOn==false) return 0;

	sound = new SoundItem();
	
	if (!sound) return 0;
	
	sound->stopit = 0;
	loadedBuffer = (unsigned char *)audioBuffer;
	loadedSize = audioLength;	

	sound->spec.format = (bitsPerSample == 8) ? AUDIO_U8 : AUDIO_S16;	
	sound->spec.channels = channels;
	sound->spec.freq = freq;

    sound->spec.callback = AudioCallback;
    sound->spec.userdata = sound;
	sound->playing = false;	
	
	int bytesPerSample = bitsPerSample / 8;
    long totalSamples = sound->audioLength / (channels * bytesPerSample);
    sound->playtime = static_cast<float>(totalSamples) / freq;	

	MSS_ConvertAudioFormat(sound);	

	sound->spec.freq = thefrequency;
	sound->spec.channels = thechannels;
	sound->spec.format = theformat;	
	
	g_FXsounds.push_back(sound);

	return sound;
}

extern "C" void* MSS_LoadStreamToSoundFX(const char *filename) 
{
	char *oggData;
    size_t fileSize = 0;
	
	if ((strstr(filename,".ogg"))||(strstr(filename,".OGG")))
	{
		oggData = MSS_LoadFileToMemory(filename, fileSize);
		if (!oggData) 
		{
			return 0;
		}
		
		SoundItem* sound = (SoundItem*)MSS_LoadStreamFromMemory(oggData, fileSize,MSS_TYPE_OGG);
		if (sound)
		{
			sound->extFile = (unsigned char *)oggData;
		}
		else
		{
			free(oggData);
		}

		return (void*)sound;
	}
	else if ((strstr(filename,".mp3"))||(strstr(filename,".MP3")))
	{
		return 0;
	}
	else if ((strstr(filename,".aiff"))||(strstr(filename,".AIFF")))
	{
		return 0;
	}
	else
	{
		return 0;
	}
}

extern "C" float MSS_GetPlaytime(void *handle)
{
	SoundItem *item = (SoundItem*)handle;
	if (!item) return 0.0f;
	
	return item->playtime;
}

extern "C" float MSS_GetCurrenttime(void *handle)
{
	SoundItem *item = (SoundItem*)handle;
	if (!item) return 0.0f;
	
	float percentage;
	
	percentage = item->position/item->audioLength;
	percentage = (float)item->playtime;
	
	return percentage;
}

extern "C" int MSS_GetLooped(void *handle)
{
	SoundItem *item = (SoundItem*)handle;
	if (!item) return 0;
	
	return item->looped;
}

extern "C" void MSS_SetLooped(void *handle, bool looped)
{
	SoundItem *item = (SoundItem *)handle;
	if (!item) return;
	
	item->looped = looped;
}

