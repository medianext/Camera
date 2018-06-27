// MFCamera.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Camera.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DllAPI __declspec(dllexport)
	

std::vector<Camera *> cameraList;


int DllAPI Init()
{
	// 初始化Media Foundation
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		hr = MFStartup(MF_VERSION);
	}
	return 0;
}

int DllAPI EnumCamera()
{
	HRESULT hr = S_OK;
	IMFAttributes *pAttributes = NULL;
	static IMFActivate **ppDevices;
	UINT32 cameraCnt = 0;

	// Enumerate devices.
	hr = MFCreateAttributes(&pAttributes, 1);
	if (SUCCEEDED(hr))
	{
		hr = pAttributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
	}

	if (SUCCEEDED(hr))
	{
		hr = MFEnumDeviceSources(pAttributes, &ppDevices, &cameraCnt);
		for (UINT32 i = 0; i < cameraCnt; i++)
		{
			Camera* camera = new Camera(ppDevices[i]);

			cameraList.push_back(camera);
		}
	}
	SafeRelease(&pAttributes);

	return cameraCnt;
}

DllAPI wchar_t* GetCameraName(int idx)
{
	if (idx >= (int)cameraList.size() || idx < 0)
		return nullptr;

	return (wchar_t*)(cameraList[idx]->Name.c_str());
}

int DllAPI OpenCamera(int idx, HWND hWnd)
{
	if (idx >= (int)cameraList.size() || idx < 0)
		return -1;
	Camera* camera = cameraList[idx];

	camera->Start(hWnd);

	return 0;
}

int DllAPI CloseCamera(int idx)
{
	if (idx >= (int)cameraList.size() || idx < 0)
		return -1;
	Camera* camera = cameraList[idx];

	camera->Stop();

	return 0;
}


#ifdef __cplusplus
}
#endif