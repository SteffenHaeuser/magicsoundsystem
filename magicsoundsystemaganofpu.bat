m68k-amigaos-gcc -DUSEAGA -DNOFPU -noixemul -m68030 -msoft-float -c magicsoundsystem.cpp -o magicsoundsystem.o
m68k-amigaos-gcc -DUSEAGA -noixemul -m68040 -msoft-float -c magicothersystem.cpp -o magicothersystem.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float -c audio_utils.cpp -o audio_utils.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float -c dll.c -o dll.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float -c dllimport.c -o dllimport.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float -c dllstartup.c -o dllstartup.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float -c dlltables.c -o dlltables.o
m68k-amigaos-gcc -noixemul -m68040 -msoft-float audio_utils.o magicsoundsystem.o magicothersystem.o dll.o dllimport.o dllstartup.o dlltables.o c2p8_040_asm.o c2p8_040_dbl_asm.o -o magicsoundsystem_aga_nofpu.dll -lSDL_AGA_060 -lstdc++ -lvorbis -lvorbisfile -logg -lvorbis -lvorbisfile -logg -ltimidity -lm