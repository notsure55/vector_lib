#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

void create_console()
{
    if (!AllocConsole())
    {
        print_error_and_exit();
    }

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
}

void cleanup(LPVOID module)
{
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)module, 0);
}

// client + 0x78A280;

DWORD WINAPI entry_point(const LPVOID module)
{
    create_console();

    while (!GetAsyncKeyState(VK_DELETE))
    {
        printf("Hey whats up we injected on some successfull sht\n");
        Sleep(200);
    }

    cleanup(module);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) {
        return false;
    }

    const HANDLE handle = CreateThread(
        NULL,
        0,
        entry_point,
        hModule,
        0,
        NULL
    );

    if (handle) {
        CloseHandle(handle);
    }

    return TRUE;
}
