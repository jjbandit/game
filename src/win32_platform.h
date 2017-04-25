#ifndef UNIX_PLATFORM_H
#define UNIX_PLATFORM_H

#include <Windows.h>
#include <WinBase.h>
#include <Wingdi.h>

#include <sys/stat.h>

#include <stdint.h>



#define Assert(condition) if (!(condition)) __debugbreak();

#ifdef __CYGWIN__

#define GAME_LIB_PATH "build/cygGame"

#else // MSVC

#define GAME_LIB_PATH "Debug/Game"
#define snprintf(...) _snprintf(__VA_ARGS__)

#endif // __CYGWIN__

#define GAME_LIB (GAME_LIB_PATH".dll")

#define EXPORT extern "C" __declspec( dllexport )

#define THREAD_MAIN_RETURN DWORD WINAPI
#define GAME_MAIN_PROC FARPROC GameMain

#define sleep(seconds) Sleep(seconds * 1000)

#define SWAP_BUFFERS SwapBuffers(hDC)

#define bonsaiGlGetProcAddress(procName) wglGetProcAddress(procName)

// #define Log(str) OutputDebugString(str)


typedef HANDLE thread_id;
typedef HANDLE semaphore;


// ???
typedef HMODULE shared_lib;
typedef HWND window;

#endif