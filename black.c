#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <Hook/TrampolineHook.h>

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

typedef void(__fastcall* create_move_t)(void* a1, int a2, char a3);
create_move_t trampoline_function = NULL;

void my_create_move(void* a1, int a2, char a3)
{

    printf("Yo insidie create_move!\n");

    trampoline_function(a1, a2, a3);
}

// client + 0x78A280;

DWORD WINAPI entry_point(const LPVOID module)
{
    create_console();

    size_t client_address = (size_t)GetModuleHandle("client.dll");
    void* create_move_address = (void*)(client_address + 0x78A280);
    printf("%zx\n", (size_t)create_move_address);

    //install_trampoline_hook((void*)&create_move_address, (void*)&my_create_move, (void**)&trampoline_function);

    while (!GetAsyncKeyState(VK_DELETE))
    {
        //printf("Hey whats up we injected on some successfull sht\n");
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
