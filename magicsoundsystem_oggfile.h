#ifndef MAGICSOUNDSYSTEM_OGGFILE_H
#define MAGICSOUNDSYSTEM_OGGFILE_H

struct MSS_OggFile
{
    char* curPtr;
    char* filePtr;
    size_t fileSize;
};

#endif