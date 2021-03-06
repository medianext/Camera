// MFCamera.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Camera.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DllAPI __declspec(dllexport)
	

	DllAPI std::vector<Camera *> cameraList;


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

	void DllAPI Uninit()
	{
		// 反初始化 Media Foundation
		MFShutdown();
		CoUninitialize();
	}

	int DllAPI EnumCamera()
	{
		HRESULT hr = S_OK;
		/*
		for (size_t i = 0; i < cameraList.size(); i++)
		{
			if (cameraList[i] != nullptr)
			{
				delete cameraList[i];
			}
		}
		//*/
		cameraList.clear();

		// Enumerate devices.
		IMFAttributes *pAttributes = NULL;
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
			UINT32 cameraCnt = 0;
			IMFActivate **ppDevices;
			hr = MFEnumDeviceSources(pAttributes, &ppDevices, &cameraCnt);
			for (UINT32 i = 0; i < cameraCnt; i++)
			{
				Camera* camera = new Camera(ppDevices[i]);
				cameraList.push_back(camera);
				SafeRelease(&ppDevices[i]);
			}
		}
		SafeRelease(&pAttributes);

		return (int)cameraList.size();
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
		if (camera != nullptr)
		{
			camera->Start(hWnd);
		}

		return 0;
	}

	int DllAPI CloseCamera(int idx)
	{
		if (idx >= (int)cameraList.size() || idx < 0)
			return -1;

		Camera* camera = cameraList[idx];
		if (camera != nullptr)
		{
			camera->Stop();
		}

		return 0;
	}

	int DllAPI Capture(int idx, byte* data, int* w, int* h)
	{
		if (idx >= (int)cameraList.size() || idx < 0)
			return -1;

		Camera* camera = cameraList[idx];
		if (camera != nullptr)
		{
			return camera->Capture(data, w, h);
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif