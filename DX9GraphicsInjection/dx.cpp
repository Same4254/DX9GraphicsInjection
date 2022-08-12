#include "pch.h"
#include "dx.hpp"

HWND DX::window;
int DX::windowWidth, DX::windowHeight;

BOOL CALLBACK DX::enumWind(HWND handle, LPARAM lp) {
	DWORD procId;
	GetWindowThreadProcessId(handle, &procId);

	if (GetCurrentProcessId() != procId)
		return true;

	window = handle;
	return false;
}

HWND DX::GetProcessWindow() {
	window = NULL;

	EnumWindows(enumWind, NULL);

	RECT size;
	GetWindowRect(window, &size);
	windowWidth = size.right - size.left;
	windowHeight = size.bottom - size.top;

	windowHeight -= 29;
	windowWidth -= 5;

	return window;
}

/**
* Given the pointer to output the vTable, and the known size of the vTable, this function will copy the contents
*	of the real vTable into the given pointer.
*
* The vTable is basically a list of function pointers. So getting this vTable will give the address of any function we wish to hook for the directx device
*
* This is done by creating a temporary dummy device, and then copying its vTable.
*
* Returns true if successful
*/
bool DX::GetD3D9DeviceVTable(void** vTable, size_t size) {
	if (!vTable)
		return false;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	IDirect3DDevice9* pDummyDevice = nullptr;

	D3DPRESENT_PARAMETERS deviceParameters = {};
	deviceParameters.Windowed = false;
	deviceParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	deviceParameters.hDeviceWindow = GetProcessWindow();

	//Create a device to get access to the vtable of the d3d9 context (they share the vtable)
	HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, deviceParameters.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &deviceParameters, &pDummyDevice);

	if (dummyDeviceCreated != S_OK) {
		deviceParameters.Windowed = !deviceParameters.Windowed;

		//Retry with both windowed and not windowed mode
		HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, deviceParameters.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &deviceParameters, &pDummyDevice);
		if (dummyDeviceCreated != S_OK) {
			pD3D->Release();
			return false;
		}
	}

	//Copy the vTable, release the creator and the dummy device
	memcpy(vTable, *(void***)(pDummyDevice), size);
	pDummyDevice->Release();
	pD3D->Release();

	return true;
}
