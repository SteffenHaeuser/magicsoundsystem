m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c magicsoundsystem.cpp -o magicsoundsystem.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c audio_utils.cpp -o audio_utils.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c dll.c -o dll.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c dllimport.c -o dllimport.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c dllstartup.c -o dllstartup.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math -c dlltables.c -o dlltables.o
m68k-amigaos-gcc -noixemul -m68040 -ffast-math audio_utils.o magicsoundsystem.o dll.o dllimport.o dllstartup.o dlltables.o -o magicsoundsystem.dll -lSDL -lstdc++ -lvorbis -lvorbisfile -logg -lvorbis -lvorbisfile -logg -ltimidity -lm