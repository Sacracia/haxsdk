#include "hax.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <strsafe.h>

namespace Hax
{
    void Panic(LogFile* logFile, const char16* fmt, ...)
    {
        char16 msgBuf[512];
        va_list args;
        va_start(args, fmt);
        ::StringCchVPrintfW(msgBuf, _countof(msgBuf), fmt, args);
        va_end(args);

        if (logFile != nullptr)
            Hax::LogError(*logFile, msgBuf);

        char16 finalBuffer[1024];
        ::StringCchPrintfW(finalBuffer, _countof(finalBuffer),
            L"Critical error!\n\n"
            L"Message: %ls\n"
            L"Application will be closed!",
            msgBuf);

        ::MessageBoxW(NULL, finalBuffer, L"HAXSDK - Assertion Failed", 
            MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_SYSTEMMODAL);

        if (::IsDebuggerPresent())
            __debugbreak();

        ::ExitProcess(1);
    }

    struct AllocHeader
    {
        size_t Size;
        size_t Magic;
    };
    static_assert(sizeof(AllocHeader) % alignof(std::max_align_t) == 0);

    Allocator g_GlobalAlloc{ "Global" };

    void* Alloc(Allocator& alloc, size_t size)
    {
        void* ptr = malloc(size + sizeof(AllocHeader));
        if (!ptr) return nullptr;

        alloc.TotalAllocated += size;
        alloc.MaxAllocated = Max(alloc.MaxAllocated, alloc.TotalAllocated);

        AllocHeader* header = (AllocHeader*)ptr;
        header->Size = size;
        header->Magic = (size_t)&alloc;

        return (void*)((uint8*)ptr + sizeof(AllocHeader));
    }

    void Free(Allocator& alloc, void* ptr)
    {
        if (!ptr) return;

        AllocHeader* header = (AllocHeader*)((uint8*)ptr - sizeof(AllocHeader));
        HAX_ASSERT(header->Magic == (size_t)&alloc);

        header->Magic = 0;
        alloc.TotalAllocated -= header->Size;

        free(header);
    }

    struct IniFileEntry
    {
        const char*         Name;
        size_t              Hash;
        void*               Data;
        IniFileWrite        WriteFn;
        IniFileRead         ReadFn;
    };

    struct IniFileSection
    {
        const char*         Name;
        size_t              Hash;
        Vector<IniFileEntry> Entries;
    };

    IniFile::~IniFile()
    {
        for (IniFileSection* section : Sections) 
            Delete(section);
    }

    void IniSave(IniFile& iniFile)
    {
        HANDLE hFile = ::CreateFileW(iniFile.FileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
            return;

        IniStringBuilder sb;
        DWORD written = 0;

        for (const IniFileSection* section : iniFile.Sections)
        {
            if (section->Entries.Empty())
                continue;

            sb.Clear();
            sb.AppendF("[%s]\r\n", section->Name);
            ::WriteFile(hFile, sb.CStr(), (DWORD)sb.SizeInBytes(), &written, nullptr);

            for (const IniFileEntry& entry : section->Entries)
            {
                sb.Clear();
                sb.AppendF("%s=", entry.Name);
                entry.WriteFn(entry.Data, sb);
                sb.Append("\r\n");
                ::WriteFile(hFile, sb.CStr(), (DWORD)sb.SizeInBytes(), &written, nullptr);
            }
        }

        ::CloseHandle(hFile);
    }

    void IniLoad(IniFile& iniFile)
    {
        HANDLE hFile = ::CreateFileW(iniFile.FileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) 
            return;

        DWORD size = ::GetFileSize(hFile, nullptr);
        if (size == 0 || size == INVALID_FILE_SIZE)
        { 
            ::CloseHandle(hFile); 
            return; 
        }

        Vector<char> buffer; 
        buffer.Resize((size_t)size + 1);
        DWORD read;
        ::ReadFile(hFile, buffer.Data(), size, &read, NULL);
        ::CloseHandle(hFile);
        buffer[size] = '\0';

        const char* fileEnd = &buffer[size];
        char* line = buffer.begin();

        IniFileSection* activeSection = nullptr;
        while (line < fileEnd)
        {
            char* lineEnd = line + 1;
            while (lineEnd < fileEnd && *lineEnd != '\n' && *lineEnd != '\r')
                ++lineEnd;
            *lineEnd = '\0';

            if (*line == '[' && *(lineEnd - 1) == ']')
            {
                size_t hash = Hash(line + 1, lineEnd - line - 2);
                for (IniFileSection* section : iniFile.Sections)
                {
                    if (section->Hash == hash)
                    {
                        activeSection = section;
                        break;
                    }
                }
            }

            char* sep = strchr(line, '=');
            if (sep && activeSection != nullptr)
            {
                *sep = '\0';
                size_t hash = Hash(line, sep - line);
                for (IniFileEntry& entry : activeSection->Entries)
                {
                    if (entry.Hash == hash && entry.ReadFn)
                    {
                        entry.ReadFn(entry.Data, sep + 1);
                        break;
                    }
                }
            }

            line = lineEnd + 1;
            while (line < fileEnd && (*line == '\n' || *line == '\r'))
                ++line;
        }
    }

    void IniAddEntry(IniFile& iniFile, const char* section, const char* name, void* data, IniFileWrite writeFn, IniFileRead readFn)
    {
        size_t sectionHash = Hash(section);

        IniFileEntry newEntry{name, Hash(name), data, writeFn, readFn};

        for (IniFileSection* section : iniFile.Sections)
        {
            if (section->Hash == sectionHash)
            {
                for (const IniFileEntry& entry : section->Entries)
                {
                    if (entry.Hash == newEntry.Hash)
                        return;
                }

                section->Entries.PushBack(newEntry);
                return;
            }
        }

        IniFileSection* newSection = New<IniFileSection>();
        newSection->Name = section;
        newSection->Hash = sectionHash;
        newSection->Entries.PushBack(newEntry);

        iniFile.Sections.PushBack(newSection);
    }

    void IniRemoveEntry(IniFile& iniFile, const char* section, const char* name)
    {
        size_t nameHash = Hash(name);
        size_t sectionHash = Hash(section);

        for (IniFileSection* section : iniFile.Sections)
        {
            if (section->Hash != sectionHash)
                continue;

            auto& entries = section->Entries;
            for (size_t i = 0; i < entries.Size(); ++i)
            {
                IniFileEntry& entry = entries[i];
                if (entry.Hash == nameHash)
                {
                    entries.Erase(i);
                    return;
                }
            }
        }
    }

    void InitLogFile(LogFile& logFile, const char16* fileName, bool useConsole)
    {
        logFile.hMutex = (Handle)::CreateMutexW(NULL, FALSE, NULL);

        wchar_t prevName[MAX_PATH];
        ::StringCchPrintfW(prevName, MAX_PATH, L"prev_%ls", fileName);

        ::DeleteFileW(prevName);
        ::MoveFileExW(fileName, prevName, MOVEFILE_REPLACE_EXISTING);

        HANDLE hFile = ::CreateFileW(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) 
        {
            logFile.hFile = (Handle)hFile;
            uint16 bom = 0xFEFF;
            DWORD written;
            ::WriteFile(hFile, &bom, sizeof(bom), &written, NULL);
        }

        if (useConsole)
        {
            ::AllocConsole();
            HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole != INVALID_HANDLE_VALUE)
            {
                logFile.hConsole = (Handle)hConsole;

                FILE* tmp;
                freopen_s(&tmp, "CONOUT$", "w", stdout);
                freopen_s(&tmp, "CONOUT$", "w", stderr);
                freopen_s(&tmp, "CONIN$", "r", stdin);

                ::SetConsoleTitleW(L"HaxSdk Log Console");
            }
        }
    }

    void CloseLogFile(LogFile& logFile)
    {
        //!
    }

    static void LogV(LogFile& logFile, const char16* tag, WORD textCol, const char16* fmt, va_list args)
    {
        if (logFile.hFile == 0 && logFile.hConsole != 0)
            return;
        
        HANDLE hFile = (HANDLE)logFile.hFile;
        HANDLE hConsole = (HANDLE)logFile.hConsole;
        HANDLE hMutex = (HANDLE)logFile.hMutex;

        char16 msgBuf[512];
        ::StringCchVPrintfW(msgBuf, _countof(msgBuf), fmt, args);

        char16 finalLine[768];
        ::StringCchPrintfW(finalLine, _countof(finalLine), L"[%-7ls:   HAXSDK] %ls\r\n", tag, msgBuf);
        size_t lineLen = ::wcslen(finalLine);

        ::WaitForSingleObject(hMutex, INFINITE);

        if (logFile.hFile != 0)
        {
            DWORD written;
            ::WriteFile(hFile, finalLine, (DWORD)(lineLen * sizeof(char16)), &written, 0);
            ::FlushFileBuffers(hFile);
        }

        if (logFile.hConsole != 0)
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            WORD oldAttributes = 0;

            if (::GetConsoleScreenBufferInfo(hConsole, &csbi))
                oldAttributes = csbi.wAttributes;

            ::SetConsoleTextAttribute(hConsole, textCol);

            DWORD written;
            ::WriteConsoleW(hConsole, finalLine, (DWORD)lineLen, &written, 0);

            if (oldAttributes != 0)
                ::SetConsoleTextAttribute(hConsole, oldAttributes);
        }

        ::ReleaseMutex(hMutex);
    }

    void Log(LogFile& logFile, const char16* fmt, ...)
    {
        va_list args; va_start(args, fmt);
        LogV(logFile, L"INFO", FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, fmt, args);
        va_end(args);
    }

    void LogDebug(LogFile& logFile, const char16* fmt, ...)
    {
        va_list args; va_start(args, fmt);
        LogV(logFile, L"DEBUG", FOREGROUND_INTENSITY, fmt, args);
        va_end(args);
    }

    void LogWarning(LogFile& logFile, const char16* fmt, ...)
    {
        va_list args; va_start(args, fmt);
        LogV(logFile, L"WARNING", FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, fmt, args);
        va_end(args);
    }

    void LogError(LogFile& logFile, const char16* fmt, ...)
    {
        va_list args; va_start(args, fmt);
        LogV(logFile, L"ERROR", FOREGROUND_INTENSITY | FOREGROUND_RED, fmt, args);
        va_end(args);
    }
}