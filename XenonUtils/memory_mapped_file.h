#pragma once

#include <filesystem>

#if defined(_WIN32)
#   include <Windows.h>
#elif defined(__SWITCH__)
#   include <cstdint>
#   include <vector>
#else
#   include <sys/mman.h>
#endif

struct MemoryMappedFile {
#if defined(_WIN32)
    HANDLE fileHandle = nullptr;
    HANDLE fileMappingHandle = nullptr;
    LPVOID fileView = nullptr;
    LARGE_INTEGER fileSize = {};
#elif defined(__SWITCH__)
    // No mmap on the Switch: the file is read into memory (as
    // UnleashedRecomp-NX does).
    std::vector<uint8_t> fileData;
    bool opened = false;
#else
    int fileHandle = -1;
    void *fileView = MAP_FAILED;
    off_t fileSize = 0;
#endif

    MemoryMappedFile();
    MemoryMappedFile(const std::filesystem::path &path);
    MemoryMappedFile(MemoryMappedFile &&other);
    ~MemoryMappedFile();
    bool open(const std::filesystem::path &path);
    void close();
    bool isOpen() const;
    uint8_t *data() const;
    size_t size() const;
};
