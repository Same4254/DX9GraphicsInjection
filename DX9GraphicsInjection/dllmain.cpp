// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>

#include "Drawing.hpp"
#include "detours.h"

// In a more final verion, I would probably create a struct for the global state rather than a bunch of gloabal fields

void* d3d9DeviceVTable[DEVICE_VTABLE_SIZE];
LPDIRECT3DDEVICE9 device = nullptr;

BeginScene originalBeginScene = nullptr;
EndScene originalEndScene = nullptr;
SetTransform originalSetTransform = nullptr;

bool viewMatrixSet = false;
D3DXMATRIX viewMatrix;

bool projectionMatrixSet = false;
D3DXMATRIX projectionMatrix;

/**
* Hooks the function that begins the drawing. Exists for debugging mainly
*/
void APIENTRY hookBeginScene(LPDIRECT3DDEVICE9 originalDevice) {
    viewMatrixSet = false;
    projectionMatrixSet = false;

    originalBeginScene(originalDevice);
}

/**
* Hooks onto the End Scene function.
* This is the function that ends drawing the scene. However, just before actually ending the scene, we draw stuff
* This also gives us the pointer to the directx device. The pointer to this function is gotten from the dummy instance of a directx device
*/
void APIENTRY hookEndScene(LPDIRECT3DDEVICE9 originalDevice) {
    if (!device) {
        std::cout << "Hooked!" << std::endl;
        device = originalDevice;

        Drawing::Init(device);
    }

    Drawing::Draw(device, projectionMatrix, viewMatrix);

    originalEndScene(originalDevice);
}

/**
* The idea is to get a pointer to the view and projection matrix by hooking onto the set transform function.
* Whenever the SetTransform function is called, we write down the pointer to the matrix if it is either the view or projection matrix
* We need the view and projection matricies for the shader
*/
void APIENTRY hookSetTransform(LPDIRECT3DDEVICE9 pDevice, D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) {
    if (State == D3DTS_VIEW && !viewMatrixSet) {
        viewMatrix = *pMatrix;
        viewMatrixSet = true;
    }
    else if (State == D3DTS_PROJECTION && !projectionMatrixSet) {
        projectionMatrix = *pMatrix;
        projectionMatrixSet = true;
    }

    originalSetTransform(pDevice, State, pMatrix);
}

DWORD WINAPI HackThread(HMODULE hModule) {
    AllocConsole();

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    // Get the vtable contents for the directx devices and hook onto the relevant functions
    if (DX::GetD3D9DeviceVTable(d3d9DeviceVTable, sizeof(d3d9DeviceVTable))) {
        // function pointer indecies gotten from the location of the function in the header
        // Not a perfect assumption but seems to work well
        originalBeginScene = (BeginScene)(d3d9DeviceVTable[41]);
        originalEndScene = (EndScene)(d3d9DeviceVTable[42]);
        originalSetTransform = (SetTransform)(d3d9DeviceVTable[44]);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)originalEndScene, hookEndScene);
        DetourAttach(&(PVOID&)originalSetTransform, hookSetTransform);
        DetourAttach(&(PVOID&)originalBeginScene, hookBeginScene);

        LONG lError = DetourTransactionCommit();
        if (lError != NO_ERROR) {
            MessageBox(HWND_DESKTOP, L"Failed to detour", L"", MB_OK);
        }
    }
    else {
        std::cout << "Failed to get the device :(" << std::endl;
    }

    while (true) {
        if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
            break;
        }

        Sleep(100);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
                           break;
    case DLL_PROCESS_DETACH:
        // Remove the trampoline hooks
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)originalBeginScene, hookBeginScene);
        DetourDetach(&(PVOID&)originalEndScene, hookEndScene);
        DetourDetach(&(PVOID&)originalSetTransform, hookSetTransform);

        LONG lError = DetourTransactionCommit();
        if (lError != NO_ERROR) {
            MessageBox(HWND_DESKTOP, L"Failed to detour", L"", MB_OK);
            return FALSE;
        }

        break;
    }
    return TRUE;
}
