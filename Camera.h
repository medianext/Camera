#pragma once

#include "Render.h"

class Camera : public IMFSourceReaderCallback
{

public:
	Camera(void * priv);
	~Camera();

	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFSourceReaderCallback methods
	STDMETHODIMP OnReadSample(
		HRESULT hrStatus,
		DWORD dwStreamIndex,
		DWORD dwStreamFlags,
		LONGLONG llTimestamp,
		IMFSample *pSample
	);

	STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
	{
		return S_OK;
	}

	STDMETHODIMP OnFlush(DWORD)
	{
		return S_OK;
	}

public:
	int Start(HWND hWnd);
	int Stop();
	int Capture(byte* data, int* w, int* h);

private:
	long m_nRefCount;        // Reference count.
	CRITICAL_SECTION m_critsec;

	IMFSourceReader *m_pReader;

	Render* render;

	bool capture;

	IMAGE_TRANSFORM_FN      m_convertFn;

	Frame* curFrame;

public:
	wstring Name;
	wstring Link;


	GUID format;
	int width;
	int stride;
	int height;
	int fps;
};

int GetDefaultStride(GUID format, int width);